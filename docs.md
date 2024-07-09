## Syntax

The syntax for macros in CrowJS is generally the same as in C++, except for the requirement that macros must be placed at the top of the file.

---

## Object-like macros
Macro names are replaced with its associated value.

Input:
```javascript
#define PI 3.14159265

console.log(PI)
```

Output:
```javascript
console.log(3.14159265)
```

---

## Function-like macros
Behaves like an object-like macro, but takes in an arbitrary number of arguments.

Input:
```javascript
#define cube(n) n**3

console.log(cube(12))
```

Output:
```javascript
console.log(12**3)
```

---

## Predefined macros
`__DATE__`: Current date (Mmm dd yyyy)  
`__TIME__`: Current time (hh:mm:ss)  
`__FILE__`: Filename of the transpiled source file  
`__LINE__`: Current line in the transpiled source file  
`__FUNCTION__`: Name of the function the macro is in, otherwise returns `null` 