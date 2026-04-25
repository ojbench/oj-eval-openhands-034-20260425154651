#include <bits/stdc++.h>
#include "SkipList.hpp"
using namespace std;

// We implement an interactive test harness compatible with potential OJ checker.
// The input format (not specified) is assumed as operations count followed by operations:
// op x: where op is 1 insert, 2 delete, 3 search; output for search is 1 or 0.
// If OJ uses different harness, they will include our header directly for template evaluation.

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int q;
    if(!(cin>>q)){
        // If no input, just exit with success
        return 0;
    }
    SkipList<long long> sl;
    while(q--){
        int t; long long x; cin>>t>>x;
        if(t==1) sl.insert(x);
        else if(t==2) sl.deleteItem(x);
        else if(t==3) cout << (sl.search(x)?1:0) << '\n';
    }
    return 0;
}
