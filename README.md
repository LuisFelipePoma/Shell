# Shell

## Files for Configuration

- `.vscode` : Configuration files (`tasks.json` and `launch.json`) for debug in vscode
- `.clangd` : Configuration file for clangd server language
- `setup.sh` : Script for create `PATHS` for llvm and antlr and activate `env`
- `requirements.txt` : File for install `antlr4` with pip
- For debugger/runner of vscode works, add this variables to your `.zshrc`, `.bashrc`, `etc`.
	```bash
	export MYDEPENDENCIES=$HOME/Apps
	export LLVMDEPENDENCIES=$HOME/llvm/llvm-project/build

	export ANTLR4HOME=$MYDEPENDENCIES/antlr4-cpp/run
	export ANTLR4I=$ANTLR4HOME/usr/local/include/antlr4-runtime
	export ANTLR4L=$ANTLR4HOME/usr/local/lib/

	export LLVM17I=$LLVMDEPENDENCIES/include
	export LLVM17L=$LLVMDEPENDENCIES/lib
	```

## Installation

### Install Antlr
 - Create a virtual enviroment
	```bash
	python -m venv /path/to/new/virtual/environment
	```
 
 - Install Antlr using pip
	```bash
	python -m pip install antlr4-tools
	```
	> or execute with the file `requirements.txt`
	```bash
	pip install -r requirements.txt
	```

 
 - Verify installation
	```bash
	antlr
	```
### Configure Antlr Runtime

1. Using CLI (Arch Linux)
	```bash
	sudo pacman -S antlr4-runtime
	```

2. Manually (Others / Debian)
	* Download `antlr4-runtime` via Curl

		```bash
		curl https://www.antlr.org/download/antlr4-cpp-runtime-4.13.1-source.zip -o antlr4-runtime.zip  
		```

	* Now install the necesary dependencies and libraries

		```bash
		sudo apt install cmake
		sudo apt install uuid-dev
		sudo apt install pkg-config 
		```

	- Now we compile and get the libraries from antlr4-runtime

		```bash
		mkdir build && mkdir run && cd build
		cmake ..
		DESTDIR=../run make install
		```

	* Now copy the ANTLR 4 include files to `/usr/local/include` and the ANTLR 4 libraries to `/usr/local/lib`

		```bash
		cd ../run/usr/local/include
		ln -s ~/Apps/antlr4-cpp/run/usr/local/include/antlr4-runtime /usr/local/include
		cd ../lib
		sudo cp * /usr/local/lib
		sudo ldconfig
		```
		> Now `antlr4-runtime` is installed
### Install llvm

- Using CLI in Arch
	```bash
	sudo pacman llvm
	```

- Using CLI in Debian
	```bash
	sudo apt-get install llvm-15 llvm-15-dev llvm-15-doc llvm-15-linker-tools llvm-15-runtime llvm-15-tools
	```

- Installing manually
	```bash
	git clone --depth 1 https://github.com/llvm/llvm-project.git
	```
	```bash
	cd llvm-project
	```
	```bash
	cmake -S llvm -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
	```
	```bash
	cmake --build build -j4
	```
	> Install in the libraries of the system
	```bash
	cmake --install build --prefix /usr/local/include
	```

## Using llvm
	
- Compiling a llvm project
	```bash
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=1
	```
	```bash
	cmake --build build
	```
	> Execute the binary file in `./build/`
	```bash
	build/prog
	```
> PoV
