Remove["Global`*"]
ClearAll["Global`*"]
(*define f*)
f[z_]=z*z/3^(1/2)+1/3;

(*define R1*)
h=1/2;
"Time for R1 : " 
R1:=(Expand[f[f[f[f[f[f[f[f[f[f[h]]]]]]]]]]];);
Print[AbsoluteTiming[R1]*1000];
Remove["Global`*"]
ClearAll["Global`*"]

(*define hermite*)
hermite[1,y_]=y/2;
hermite[0,y_]=1;
hermite[n_,y_]:=Expand[2*y*hermite[n-1,y]-2*(n-1)*hermite[n-2,y]];

(*define R2*)
R2[n_,y_]:=hermite[n,y];
n=15;
"Time for R2 : " 
Print[1000*AbsoluteTiming[R2[n,y];]];
Remove["Global`*"]
ClearAll["Global`*"]

(*define R3*)
f[]=x+y+z;
Array[fc, 10];
"Time for R3: " 
Print[AbsoluteTiming[Do[fc[i] =Boole[(f[]==f[])],{i,1,10}]]*1000]

(*define R7*)
"Time for R7: " 
f[x_]=x^24+34* x^12+45*x^3+9* x^18+34* x^10+32*x^21;
Print[AbsoluteTiming[Do[f[0.5], 10000]*1000]]

(*define S3a*)
f[x_] = Expand[(x^y+ y^z+ z^x)^(500)];
"Time for S3a: " 
Print[AbsoluteTiming[f'[x];]*1000];
Remove["Global`*"]
ClearAll["Global`*"]

(*define S3*)
f[x_] = Expand[(x^y+ y^z+ z^ x)^ 50];
"Time for S3:" 
Print[AbsoluteTiming[f'[x];]*1000];
Remove["Global`*"]
ClearAll["Global`*"];

(*define S2*)
"Time for S2: " 
Print[AbsoluteTiming[Expand[(x^sin(x)+ y^ cos(y)+z^(x+y))^100];]*1000]
Remove["Global`*"]
ClearAll["Global`*"]

(*S1*)
f = (x+y+z+1)^ 7;
e=f*(f+1);
"Time for S1" 
Print[AbsoluteTiming[Expand[e];]*1000];
Remove["Global`*"]
ClearAll["Global`*"]