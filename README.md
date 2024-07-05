<p align="center"><img src="logo.png" width=200></p>

CrowJS is a JavaScript superset that has C++ style macros, which can provide convenience when programming by reducing repetitive code.


## Getting Started
Write your code
```javascript
// myfile.js
#define MACRO console.log(__DATE__)

MACRO;
```

Transpile to JavaScript
```batch
> crow myfile.js
Transpiled to myfile.js
Process exited after 0.457053 seconds
```


## See more
[Documentation](documentation.md)
