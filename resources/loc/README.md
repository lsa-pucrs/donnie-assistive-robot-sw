# Localization Files

This package supports language localization in US english and BR portuguese.
The translation files found here were generated using the [_gettext_](http://www.gnu.org/software/gettext/) package.
To select the language, set the environmental variable _DONNIE_LANG_ to either "en_US" or "pt_BR".

## Adding new messages and updating localization files

When a new log, command or debug message is added to the GoDonnie interpreter's source code, the localization files have to be updated to handle that new message. The steps below show how to update these files, using the "Compiler.cpp" source code as an example.

1. Update the .pot translation template file, located in "/pot":
```
$ xgettext --keyword=translate --language=C++ --sort-output --from-code=UTF-8 -o ./pot/Compiler.pot ../../client/godonnie/Compiler.cpp
```

2. Merge the new .pot file with the existing language-specific .po translation files:
```
$ msgmerge --update ./<en_US or pt_BR>/po/Compiler.po ./pot/Compiler.pot
```

3. Review and write the new translated messages on the language-specific .po files.

**NOTE**: Usually new messages will be marked with the “#, fuzzy” comment, which means they have been marked for revision. Delete these comments as you review the file, as the new messages will not be included when fuzzy.

4. Generate the new .mo binary translation file:
```
$ msgfmt --output-file=./<en_US or pt_BR>/LC_MESSAGES/Compiler.mo ./<en_US or pt_BR>/po/Compiler.po
```

5. Recompile donnie and install (i.e., ```sudo make install``` on build folder), so that the updated files can be installed.

## Disclaimer

- **This is a work in progress**. Feel free to open issues regarding errors in localization (e.g. mistranslations, missing terms).

## Contributors

- Renan Maidana
