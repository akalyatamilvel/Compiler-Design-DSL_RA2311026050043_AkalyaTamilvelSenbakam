# test3.dsl – semantic error: 'b' used before assignment

a = 10;
c = a + b;   # ERROR: b is not defined yet
