


import Foundation
struct BIG{ var w=[Int32](repeating: 0,count: 10); init() {}}
struct FP{var x:BIG; var xes:Int32;init(){x=BIG();xes=1}}
struct FP2{var a:FP;var b:FP;init(){a=FP();b=FP()}}
struct FP4 {var a:FP2;var b:FP2;init(){a=FP2();b=FP2()}}
struct ECP4 {var x:FP4;var y:FP4;var z:FP4;init(){x=FP4();y=FP4();z=FP4()}}
let Q=ECP4();
print("finished")
