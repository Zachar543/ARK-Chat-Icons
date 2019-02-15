# Chat Icons
Allows changing player chat icons by group or steam id

### Example Configuration
```js
{
  "Groups": { // Groups from the Permissions plugin
    "Admins": "/Game/PrimalEarth/UI/Textures/ServerAdminChat_Icon.ServerAdminChat_Icon", // Keep this line unless you want to override/remove the default ServerAdmin icon
    "Donor": "/Game/PrimalEarth/CoreBlueprints/StatusIcons/Mating_Icon.Mating_Icon"
  },
  "TribeIds": {
    "123456790": {
      "SomeTribeRank1": "/Game/PrimalEarth/UI/Textures/WildcardAdminChat_Icon.WildcardAdminChat_Icon",
      "SomeTribeRank2": "/Game/PrimalEarth/UI/Textures/StatsIcons/Fortitude_Icon.Fortitude_Icon"
    }
  },
  "SteamIds": {
    "1234567890": "/Game/Aberration/Icons/Resources/Gem3_Icon.Gem3_Icon"
  },
  "Debug": false // Settting to true will print debug messages. You probably don't need or care about this.
}
```

### Notes
* Only icons that exist within ARK or from Mods can be used.
* If a player is matched against multiple icon entries (e.g. Group and SteamId), the first entry matched in this order will be used:
  * Steam Id
  * Tribe Id (and rank)
  * Permission Group
* Removing the "Admins" key from the Groups section will disable the default ServerAdmin tag. Similarly, specifying a different icon for "Admins" will override the default ServerAdmin tag.

### Console Commands
* ChatIcons.Reload - Reloads the config
