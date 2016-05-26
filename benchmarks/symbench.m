ClearAll["Global`*"];
(*define f*)
f[z_]=z*z/3^(1/2)+1/3;

(*define R1*)
h=1/2;
"Time for R1 : " 
R1:=(Expand[f[f[f[f[f[f[f[f[f[f[h]]]]]]]]]]];);
Print[AbsoluteTiming[R1]*1000];


(*define hermite*)
hermite[1,y_]=y/2;
hermite[0,y_]=1;
hermite[n_,y_]:=Expand[2*y*hermite[n-1,y]-2*(n-1)*hermite[n-2,y]];

(*define R2*)
R2[n_,y_]:=hermite[n,y];
n=15;
Print[1000*AbsoluteTiming[R2[n,y];]];