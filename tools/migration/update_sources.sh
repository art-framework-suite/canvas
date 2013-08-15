#!/bin/bash
# Program name
prog=${0##*/}
# ======================================================================
function usage() {
    cat 1>&2 <<EOF
usage: $prog [-a|--all-lumi-cases] [--no-fix-pset] [--one-file <file>] <top-dir>
EOF
}

function one_file_lumi() {
  local F=$1
  grep -Il subrun "${F}" >/dev/null 2>&1 && return 0 # Already done
  cp -p "${F}" "$TMP" # Make sure permissions are correct on temporary file
  err=$(perl -wp -f fix-lumi.pl "${F}" 2>&1 >"$TMP" ) # Yes, the redirections are in the right order.
  status=$?
  if (( $status )); then # Oops
    echo "PROBLEM: fix manually ($(echo \"$err\" | sed -e 's/.*FATAL: //' ))!" 1>&2
    rm -f "$TMP"
    return 1
  else
      # Success!
      mv -f "$TMP" "$F"
  fi

  # Lumi fix for file name
  local Fnew=$(echo "$F" | perl -wp -f fix-lumi.pl 2>/dev/null)
  if [[ -n "$Fnew" ]] && [[ "$Fnew" != "$F" ]]; then
      if [[ -e "$Fnew" ]]; then # Oops
          echo "Unable to rename \"$F\" to already-existing file \"$Fnew\"" 1>&2
      else
          mv "$F" "$Fnew"
      fi
  fi
}

function one_file() {
  local F=$1
  printf "$F ... "
  # Fix most includes
  ed "$F" < pop.ed > /dev/null 2>&1
  # Optionally fix whitespace
  (( ${fix_whitespace:-0} )) && ed "$F" < fix-whitespace.ed > /dev/null 2>&1
  # Account for two additional moved files
  ed "$F" < movedfile_20100901_112607.ed > /dev/null 2>&1
  ed "$F" < movedfile_20100907_152621.ed > /dev/null 2>&1
  # Fix includes in and of .icc files
  perl -wapi\~ -f fix-icc-includes.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"
  # Fix edm and cms namespaces
  perl -wapi\~ -f fix-namespaces-1.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"
  # Fix ParameterSet calls
  if (( ${fix_pset:-1} )); then
    perl -wapi\~ -f fix-ParameterSet.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"
  fi
  # Fix Service -> ServiceHandle
  perl -wapi\~ -f fix-ServiceHandle.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"
  # Fix for relocated services
  perl -wapi\~ -f fix-services.pl "${F}"
  # Fix for relocated services
  perl -wapi\~ -f fix-macros.pl "${F}"
  # Fix for relocated ROOT IO files.
  perl -wapi\~ -f fix-rootIO.pl "${F}"
  # Fix use of exceptions (must fix only once due to hysteresis).
  [[ "${F}" == *"art/Utilities/Exception.cc" ]] || \
    grep -vi exception "${F}" >/dev/null 2>&1 || \
    grep -e 'cetlib/exception' \
         -e 'namespace[ \t]\{1,\}cet' \
         -e '\(art::\)\{0,1\}Exception\([ \t]\{1,\}[A-Za-z0-9_]*\([ \t]*=[ \t]*\)\{0,1\}\)\{0,1\}([ \t]*[A-Za-z0-9_]\{1,\}' \
         -e 'art::Exception' \
         "${F}" >/dev/null 2>&1 || \
      printf ' (would fix exceptions) ' || \
    { perl -wapi\~ -f fix-exceptions.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"; }
  # Final namespace fix (must be done after exception fix).
  perl -wapi\~ -f fix-namespaces-2.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"
  perl -wapi\~ -f fix-messagefacility.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"

  perl -wapi\~ -f fix-services.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"
  perl -wapi\~ -f fix-header-locs.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"

  # ReaderSource -> Source.
  perl -wapi\~ -f fix-ReaderSource.pl "${F}" >/dev/null 2>&1 && rm -f "${F}~"

  # "lumi|luminosty|luminosityblock" -> subrun
  if one_file_lumi "$F"; then
    echo "OK"
  fi
}

# ======================================================================
# Prepare:
getopt -T >/dev/null 2>&1
if (( $? != 4 )); then
  echo "ERROR: GNU getopt required! Check SETUP_GETOPT and PATH." 1>&2
  exit 1
fi

TEMP=`getopt -n "$prog" -o a --long all-lumi-cases --long one-file: --long no-fix-pset -- "${@}"`
eval set -- "$TEMP"
while true; do
  case $1 in
    -a|--all-lumi-cases)
      all_lumi=1
      shift
      ;;
    --fix-whitespace)
      fix_whitespace=1
      shift
      ;;
    --no-fix-pset)
      fix_pset=0
      shift
      ;;
    --one-file)
      file=$2
      shift 2
      ;;
    --)
      shift
      break
      ;;
    *)
      echo "Bad argument \"$OPT\"" 1>&2
      usage
      exit 1
    esac
done

(( ${all_lumi:-0} )) || export FIX_LUMI_CLASSES_ONLY=1
TOP=${1:-ART}

# ======================================================================
# Run scripts to update

TMP=`mktemp -t update_sources.sh.XXXXXX`
trap "rm $TMP* 2>/dev/null" EXIT

if [[ -n "${file}" ]]; then
  if ! [[ -r "${file}" ]]; then
    echo "ERROR: ${file} does not exist or is not readable." 1>&2
    exit 1
  else
    one_file "$file"
  fi
else
  for F in `find $TOP \( -type d \( -name .git -o -name .svn -o -name CVS -o -name ups -o -path '*/tools/migration' -o -path '*/art/ParameterSet' \) \
                       -prune \) -o -type f \! \( -name '*~' -o -name '*.bak' -o -name '*.orig' -o -name '*.new' -o -name '*.gdml' -o -name '*.py' -o -name '*.sh' -o -name '*.o' -o -name '*.so' -o -name '*.os' -o -name '*.root' -o -name 'SConscript' \) -print`; do
    one_file "$F"
  done
fi
