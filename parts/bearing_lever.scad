module cylinder_outer(height,radius,center, fn){
   fudge = 1/cos(180/fn);
   cylinder(h=height,r=radius*fudge,center=center, fn=fn );
 }

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


function lerp( a, b, w ) = (a*w + b*(1-w));

// Quality of cylinders/spheres
$fa = 10;
$fs = 0.4;
$fn = 60;

// Dimensions of the design (mm)
di = 32.0; // Inner diameter of the ring
do = 50.0; // Outer diameter of the ring
db = 40.0; // Boss diameter
h0 = 3.5; // Height of the base
hb = 6.5; // Height of the boss 
lt = 30.0; // Length of the tab (from the edge of the ring)
wt = 10.0; // Width of the tip of the tab

// For the ball-chain anchor
ds = 4.6; // Diameter of the balls 
dl = 1.5; // Diameter of the links
ll = 6.1; // Link length (distance between two ball centers)
n  = 3; // Number of pockets to hold balls

// Screw options
screw_diameter = 3.05;
screw_count = 2;
nut_diameter = 7.0; // correct?
nut_height = 2.5; // correct?


difference() {
    union() {
        // Main ring and 'boss'
        cylinder( h=h0, d=do, center=false);
        translate( [0,0,h0] ) cylinder( hb, d=db, center=false );
        
        // Construct the tapered tab along the x-axis 
        len =do/2+lt;
        segm =20;
        e =2;
        for( i = [segm-5:-1:0] ) {
            dif =i/segm;
            w2 =lerp( do, wt, pow((i+1)/segm,e));
            w1 =lerp( do, wt, pow(dif,e) );
            translate( [-len*(1.0-dif),-w1/2, 0] ) //cube( [.5, w, h0] );
                box( h=h0, w1=w1, w2=w2, l=len/segm);
         }
         
         // Screw attachments
         translate( [0,0,h0+hb/2] ) {
             intersection() {
                 cylinder( h=hb, d=do, center=true );
                 for (i = [1: screw_count]) {
                    rotate ([0, 0, 90+360/screw_count * i]) {
                        cube( [do, nut_diameter*1.5, hb], center =true );
                    }
                 }
             }
         }
        

    }
    
    // Screw holes
    translate ([0, 0, h0 + hb/4])
    for (i = [1: screw_count]) {
        rotate ([0, 0, 90+360/screw_count * i]) {
            translate ([db/2, 0, 0])
            rotate ([0 ,90 , 0])
            cylinder (db, r = screw_diameter/2, center = true);
            
            translate ([di/2 + (do/2 - di/2)/2 - nut_height/4, 0, 0]) {
                rotate ([0 , 90, 0])
                cylinder (nut_height, r = nut_diameter/2, center = true, $fn = 6);
                
                translate ([0, 0, hb / 2])
                cube ([nut_height, sin(60) * nut_diameter, hb], center = true);
            }
        }
    }

    // Cut-out with the inner diameter
    cylinder_outer( height=h0+hb, radius=di/2, center=false, fn=$fn );
    
    // Ball-chain anchor
    translate( [-do/2-lt,0,h0/2] ) {
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
    }
    
}
