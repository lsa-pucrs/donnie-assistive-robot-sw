# Localization Files

This package supports language localization in US english and BR portuguese.
The translation files found here were generated using the [_gettext_](http://www.gnu.org/software/gettext/) package.
To select the language, set the environmental variable _DONNIE_LANG_ to either "en_US" or "pt_BR".

## Disclaimer

- **Do not remove this folder from the repository's root directory**. The translator environment in the different modules expect the files to be located here.
- **This is a work in progress**. Feel free to open issues regarding errors in localization (e.g. mistranslations, missing terms).

## TODO

`l10n = localization`
`i18n = internationalization`

- l10n the commands in the GoDonnie interpreter
- i18n of drivers (donnie.cc, protocol.cc, client/manual_control.cc)
- i18n of soxplayer (soxplayer.cc)
- i18n of raspicamdriver (client/main.cc, Raspicam_driver.cc)
- i18n of genericblobfinder (GenericBlobfinder.cc)
- i18n of firmware (protocol.cpp, Ranger.cpp, sensors.cpp)

## Contributors

- Renan Maidana