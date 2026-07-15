# Epi-Template-CPP

Before doing anything you have to do ```make start``` to modified the name of gitignore.txt to .gitignore to avoid any problem with git and make gitignore work fine.

For compilation of lib/my make : ```make``` if you can't because it say "Nothing to be done for `all'" make : ```make re``` it should recompile as expected.

For clean make : ```make clean``` to clean the lib/my and for everywhere make : ```make fclean```

For coding-style : ```make cs``` Warning : You have to recompile the lib/my before any knew compilation of your project

For unit_tests (create and run them) make : ```make unit_tests & make tests_run```