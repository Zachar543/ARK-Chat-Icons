# ARK Chat Icons
Allows changing player chat icons by group or steam id

### Example Configuration
```js
{
  "Groups": { // Groups from the Permissions plugin
    "Admins": "/Game/PrimalEarth/UI/Textures/ServerAdminChat_Icon.ServerAdminChat_Icon", // Keep this line unless you want to override/remove the default ServerAdmin icon
    "Donor": "/Game/PrimalEarth/CoreBlueprints/StatusIcons/Mating_Icon.Mating_Icon"
  },
  "SteamIds": {
    "1234567890": "/Game/Aberration/Icons/Resources/Gem3_Icon.Gem3_Icon"
  },
  "InterceptGlobalChat": true, // Setting to false will use default ARK Global Chat behavior
  "InterceptTribeChat": true, // Setting to false will use default ARK Tribe Chat behavior
  "InterceptAllianceChat": true, // Setting to false will use default ARK Alliance Chat behavior
  "InterceptLocalChat": true, // Setting to false will use default ARK Local Chat behavior
  "LocalChatDistance": 4000 // Close to default ARK Local Chat distance
}
```

### Notes
* Only icons that exist within ARK or from Mods can be used.
* Values in the SteamIds list take priority over the Groups list if a player is matched in both sections.
* Removing the "Admins" key from the Groups section will disable the default ServerAdmin tag. Similarly, specifying a different icon for "Admins" will override the default ServerAdmin tag.

### Console Commands
* ArkChatIcons.Reload - Reloads the config
