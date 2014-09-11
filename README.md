Typescript.NET
==============

A LALR parser + SDT scheme = A compiler from Typescript to C#, thus to .NET

## Usage

Make sure you have rights to read and write files (admin in windows). All you need to do is:

```
tsnet my_typescript_file.ts
```

The parser requires a big chunk of precomputed data in order to work. If that data is missing, it will first generate it,
which might take some time.

## Unsupported language features
The translator is missing several features, namely:

* Generic classes and functions
* Code that is not in a class (i.e. the translator's not JS-compliant)

It is also worth mentioning that the translator is just that - a translator. It provides neither syntactic, nor semantic checking.