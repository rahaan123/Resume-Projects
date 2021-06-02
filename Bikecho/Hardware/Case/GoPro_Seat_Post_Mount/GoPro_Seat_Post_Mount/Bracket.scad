$fn=50;

// The radius of the bar the bracket should fit.
bar_r = 15.5;

// 3mm diameter hole
hinge_hole_rad = 1.8;

// 6.5mm diameter hole
clasp_hole_rad = 3.5;

module Bracket()
{
	// bracket
	union()
	{
		// half moon shape
		difference()
		{
			cylinder(12, bar_r + 5, bar_r + 5);
			translate([0,0,-10]) cylinder(30, bar_r, bar_r);
			translate([-((bar_r  * 3) / 2),-1,-15]) cube([bar_r * 3,bar_r * 3,40]);
			translate([bar_r - 3.5,-5,6]) cube([10,6,16]);	
		}

		// hinge tab
		difference()
		{
			translate([bar_r + 7,0,0]) cylinder(6,5,5);
			translate([bar_r + 7,0,-5]) cylinder(20,hinge_hole_rad,hinge_hole_rad);
		}

		// clasp tab
		difference()
		{
			union()
			{
				translate([-(bar_r + 10),-6,0]) cube([10,5,12]);
				translate([-(bar_r + 10),-1,6]) rotate([90,0,0]) cylinder(5,6,6);
			}

			translate([-(bar_r + 9.5),10,6]) rotate([90,0,0]) cylinder(20,clasp_hole_rad,clasp_hole_rad);
		}
	}
}

//Bracket();
