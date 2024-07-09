<img src="resources/logo-big.png" width=400>

CrowJS is a JavaScript superset that has C++ style macros, seeking to provide convenience when programming by reducing repetitive code.


## Getting started

Set up CrowJS by running the `build.bat` file
```batch
> build.bat
```

Write your code
```javascript
// myfile.js
#define MACRO console.log(__DATE__)

MACRO;
```

Transpile to JavaScript
```batch
> crow myfile.js
Transpiled to test.js
Process exited after 0.056123 seconds
```


## See more

- [Documentation](docs.md)
- [C++ macros](https://www.geeksforgeeks.org/cpp-macros/) 
- https://www.youtube.com/watch?v=uOiccQx20d8
