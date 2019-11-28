color("red")
        translate([-33, -50, 0])
        union() {
            translate([0, 0, 0])
            base();
            
            translate([60, 0, 5])
            coso();
            translate([31, 0, 5])
            coso();
            
            translate([42.06, 28, 2])
            base_tornillos();
            translate([14.12, 28, 2])
            base_tornillos();
            translate([56.96, 78.8, 2])
            base_tornillos();
            translate([9.04, 78.8, 2])
            base_tornillos();
        }

module base()
        {
            difference() {
                linear_extrude(height = 5)
                square([66, 99.4]);
                translate([3, 22, 2])
                linear_extrude(height = 5)
                square([60, 74.4]);
                translate([25, 4, 0])
                cylinder(d=4, h=5);
                translate([25, 14, 0])
                cylinder(d=4, h=5);
            }
        }

module coso()
        {
            difference() {
                linear_extrude(height = 12)
                square([6, 10]);
                translate([3, 0, 6])
                rotate([270, 90, 0]) cylinder(d=1.5, h=5);
            }
        }

module base_tornillos()
        {
            difference() {
                linear_extrude(height = 2)
                circle(2.5);
                cylinder(d=1.5, h=2);
            }
        }
        
