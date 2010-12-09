use strict;
use vars qw(%translations);

BEGIN { %translations = (
	  artVersionTest => "art_Version_Test",
	  artVersion => "art_Version",
	  artUtilitiesTest => "art_Utilities_Test",
	  artUtilities => "art_Utilities",
	  artParameterSetTest => "art_ParameterSet_Test",
	  artParameterSet => "art_ParameterSet",
	  artFrameworkPluginManagerTest => "art_Framework_PluginManager_Test",
	  artFrameworkPluginManager => "art_Framework_PluginManager",
	  artFrameworkCoreTest => "art_Framework_Core_Test",
	  artFrameworkCore => "art_Framework_Core",
	  artFrameworkModulesTest => "art_Framework_Modules_Test",
	  artFrameworkModules => "art_Framework_Modules",
	  artFrameworkIOCatalogTest => "art_Framework_IO_Catalog_Test",
	  artFrameworkIOCatalog => "art_Framework_IO_Catalog",
	  artFrameworkIOCommonTest => "art_Framework_IO_Common_Test",
	  artFrameworkIOCommon => "art_Framework_IO_Common",
	  artFrameworkIOInputTest => "art_Framework_IO_Input_Test",
	  artFrameworkIOInput => "art_Framework_IO_Input",
	  artFrameworkIOOutputTest => "art_Framework_IO_Output_Test",
	  artFrameworkIOOutput => "art_Framework_IO_Output",
	  artFrameworkIOSourcesTest => "art_Framework_IO_Sources_Test",
	  artFrameworkIOSources => "art_Framework_IO_Sources",
	  artFrameworkServicesBasicTest => "art_Framework_Services_Basic_Test",
	  artFrameworkServicesBasic => "art_Framework_Services_Basic",
	  artFrameworkServicesPrescaleTest => "art_Framework_Services_Prescale_Test",
	  artFrameworkServicesPrescale => "art_Framework_Services_Prescale",
	  artFrameworkServicesRegistryTest => "art_Framework_Services_Registry_Test",
	  artFrameworkServicesRegistry => "art_Framework_Services_Registry",
	  artFrameworkServicesRootAutoLibraryLoaderTest => "art_Framework_Services_RootAutoLibraryLoader_Test",
	  artFrameworkServicesRootAutoLibraryLoader => "art_Framework_Services_RootAutoLibraryLoader",
	 );
      }

foreach my $inc (reverse sort keys %translations) {
  while (s&${inc}&$translations{$inc}&g) {};
}

### Local Variables:
### mode: cperl
### End:
