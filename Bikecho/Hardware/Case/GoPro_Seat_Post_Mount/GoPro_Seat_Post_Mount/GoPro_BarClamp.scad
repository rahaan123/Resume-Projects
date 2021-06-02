include <GoPro_Connection.scad>
include <Bracket.scad>

$fn=50;

// *************************************************************************
// Change this value to match the radius of the bar you're wanting to clamp.
bar_r = 15.5;
// *************************************************************************


// ******** GoPro Connection ********
// Here, I boolean a box out of the shape of the connection piece so that the camera
// has enough clearence to standup 90 degrees to the clamp.
// ********************************** 
difference()
{
	union()
	{
		// GoPro connection
		translate([(15.8 / 2),-(bar_r + 10),0])
		{
			rotate([0,0,180])
			{
				GoPro_Connection();
			}
		}

		// Small cube to extend the connection out away from the clamp
		translate([-(15.8 / 2),-(bar_r + 10),0]) 
		{
			cube([15.8,10,10]);
		}

	}
	
	// Small cube that is cutout of the connection
	translate([-10,-(bar_r + 16.5),7]) cube([20,15,10]);
}

Bracket();

// second bracket without the GoPro connection
translate([0,(bar_r * 2),0])
{
	Bracket();
}




