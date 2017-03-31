All modules are automatically added to the project after running CMake, and the init() function of each will be called in turn.
It is up to the user to set up a task to ensure that the module continues to run. The scheduler begins after all modules have initialised.

To generate a new module, run "python create_module.py <module_name>" from the root directory of the project.
