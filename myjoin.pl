myjoin(D,A,B,C,E) :-
    file1(A,B),
    file2(A,C),
    file3(A,D),
    file4(D,E).

file1(a,b).
file1(a,c).
file1(d,e).
file1(d,f).

file2(a,g).
file2(a,h).
file2(d,i).
file2(d,j).

file3(a,k).
file3(a,l).
file3(d,m).
file3(d,k).

file4(k,n).
file4(k,o).
file4(m,p).
file4(m,q).
