# HadronsApi

Simple c++ library to build [Hadrons](https://github.com/aportelli/Hadrons) applications.
Build the project simply by cloning the github repo:

```
git clone https://github.com/ale-barone/HadronsAPI myproject
```

and checkout to the RHQTwist branch

## Usage
- copy the folder MRHQ in the Hadrons path Hadrons/Modules to overwrite the existing MRHQ modules
- recompile Hadrons (remember to run Hadrons/make_module_list.sh to update the modules to include the new MRHQSeqSource* modules)
- modify build_app.sh with your paths and run it;
- modify the main_template.cpp inside the folder app/template;
- build the xml running ./make_template_xml.sh in the folder app/template.
