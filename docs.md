## Syntax

The syntax for macros in CrowJS is essentially the same as in C++, except for two changes.
- Macros must be declared at the top of the file
- There are no variadic macro functions

---

## Object-like macros
Macro names are replaced with its associated value.

Input:
```javascript
#define PI 3.141592654
#define TAU 2*PI

console.log(PI)
console.log(TAU)
```

Output:
```javascript
console.log(3.141592654)
console.log(2*3.141592654)
```

</br>

CrowJS also works with string interpolation.

Input
```javascript
#define x 1

`${
    x+"x"+`${x}`
} ${x}`
```

Output
```javascript
`${
    1+"x"+`${1}`
} ${1}`
```

---

## Function-like macros
Behaves like an object-like macro, but takes in an arbitrary number of arguments.

Input:
```javascript
#define pow(a, b) a**b

console.log(pow(2, 3))
```

Output:
```javascript
console.log(2**3)
```

---

## Predefined macros
`__DATE__`: Current date (Mmm dd yyyy)  
`__TIME__`: Current time (hh:mm:ss)  
`__FILE__`: Filename of the transpiled source file  
`__LINE__`: Current line in the transpiled source file  
`__FUNCTION__`: Name of the function the macro is in, otherwise returns `null` 