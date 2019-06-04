
package org.apache.milagro.amcl.XXX;

public final class private_key
{
    public FF p,q,dp,dq,c;
	
	public private_key(int n)
	{
		p=new FF(n);
		q=new FF(n);
		dp=new FF(n);
		dq=new FF(n);
		c=new FF(n);
	}
}