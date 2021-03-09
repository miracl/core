// const generic example - Rust 1.51 (nightly for now)
// variable length octet from the stack - no more buffer overflows
use std::ops::{Index,IndexMut};

pub struct Oct<const N:usize> {
    val:[u8; N],
    len: usize,
}

impl<const N:usize> Oct<N> {
// Constructor
    pub fn new() -> Oct<N> {
        Oct {
            val: [0;N],
            len: 0,
        }
    }
    pub fn somefn(&mut self) {
        self.val[42]=3;
    }

    pub fn size(&self) -> usize {
        N
    }

    pub fn len(&self) -> usize {
        self.len
    }

    pub fn push(&mut self,x: u8) {
        if self.len<N {
            self.val[self.len]=x;
            self.len += 1;
        }
    }

    pub fn pop(&mut self) -> u8{
        let mut x:u8=0;
        if self.len>0 {
            x=self.val[self.len-1];
            self.len-=1;
        }
        x
    }

    pub fn clear(&mut self) {
        for i in 0..self.len {
            self.val[i]=0;
        }
        self.len=0;
    }

    pub fn copy(&mut self,b: &Oct<N>) {
        self.clear();
        for i in 0..b.len {
            self.val[i]=b.val[i];
        }
        self.len=b.len;
    }
    
    pub fn decr(&mut self) -> usize {
        let mut cs:Oct<N>=Oct::new();

        println!("cs.len = {}",cs.len);
        println!("self.len= {}",self.len);

        cs.copy(self);
        let mut tot=0;
        for i in 0..cs.len {
            tot+=cs.val[i] as usize;
        }
        return tot;
    }
}

impl<const N:usize> Index<usize> for Oct<N> 
{
    type Output = <[u8] as Index<usize>>::Output;
    fn index(&self,i:usize) -> &u8 { 
        &self.val[i] 
    } 
}

impl<const N:usize> IndexMut<usize> for Oct<N> 
{
    fn index_mut(&mut self,i:usize) -> &mut u8 { 
        if i>= self.len {
            self.len=i+1;
        }
        &mut self.val[i] 
    } 
}

fn anotherfn<const N:usize>(o: &mut Oct<N>) {
    o.val[42]=43;
}

fn main() {
    let mut info=Oct::<64>::new();
//    let mut info:Oct<64>=Oct::new(); // create u8 array of max length 64 
    let mut b:Oct<64>=Oct::new();
    for i in 0..48 {
        info.push(i as u8);
    }

    println!("{}",info.len());
    println!("{}",info[42]);
    println!("{}",info.size());
    info.somefn();
    println!("{}",info[42]);
    anotherfn(&mut info);
    println!("{}",info[42]);
    info[42]=3;
    println!("{}",info[42]);
    // info[65]=7;     // runtime error
    b.copy(&info);
    println!("{}",b[42]);
    info.decr();

//    let mut arr = Oct::<32>::new();
}
