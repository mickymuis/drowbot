module box( h, w1, w2, l ) {
    CubePoints = [
      [  0,  0,  0 ],  //0
      [ l,  w1/2-w2/2,  0 ],  //1
      [ l,  w1/2+w2/2,  0 ],  //2
      [  0,  w1,  0 ],  //3
      [  0,  0,  h ],  //4
      [ l,  w1/2-w2/2,  h ],  //5
      [ l,  w1/2+w2/2,  h ],  //6
      [  0,  w1,  h ]]; //7
      
    CubeFaces = [
      [0,1,2,3],  // bottom
      [4,5,1,0],  // front
      [7,6,5,4],  // top
      [5,6,2,1],  // right
      [6,7,3,2],  // back
      [7,4,0,3]]; // left    
    polyhedron( CubePoints, CubeFaces );
}

// Quality of cylinders/spheres
$fa = 10;
$fs = 0.4;
$fn = 60;

// Parameters
h0 = 4; // Height of the base
w1 = 12; // Width at the top
w2 = 18; // Width at the bottom
ds = 4.6; // Diameter of the balls 
dl = 1.1; // Diameter of the links
ll = 6.1; // Link length (distance between two ball centers)
n  = 3; // Number of pockets to hold balls
de = 5.05; // Diameter of the 'eye'

// Compute the length
l  = (n)*ll + de*1.5;

difference() {
    
    // Base
    minkowski() {
        translate([2,2,0]) box( h = h0/2, w1 = w1-4, w2 = w2-4, l = l-4);
        cylinder( r=2, h= h0/2 );
    }
    %box( h = h0, w1 = w1, w2 = w2, l = l);
    
    // Ball holes
    for( i = [0:n-1] ) {
        translate([i*ll-ds/4,w1/2,h0/2]) {
            sphere( d=ds );
            cube( [ll+.5, dl, h0], center=true );
            if( i != 0 ) {
                translate( [ds/4+ds/8,0,0] )
                    cube( [ds/2+ds/4,ds,h0], center =true );
            }
        }
    }
    
    // Eyelet
    translate([(((n-.5)*ll)+l)/2, w1/2, h0/2])
        cylinder( h = h0, d = de, center=true );
}