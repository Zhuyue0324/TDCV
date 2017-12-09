syms x y z
M = [1 2 3 4;
    4 5 5 7;
    1 2 3 5];

X = [x y z 1];
a = X';
f = M * a;

j = jacobian(f, [x, y, z]);