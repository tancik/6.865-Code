function writeCppSegment(s, Px,Py,Qx,Qy) {
    s = s + ".push_back(Segment(Vec2f("+Px+", "+Py+"), Vec2f("+Qx+
    ", "+Qy+"))); <br />";
    return s;
}

function getPos(e) {
    var posx = 0;
    var posy = 0;
    if (!e) e = window.event;
    if (e.pageX || e.pageY) 	{
        posx = e.pageX;
        posy = e.pageY;
    }
    else if (e.clientX || e.clientY) 	{
        posx = e.clientX + document.body.scrollLeft +
            document.documentElement.scrollLeft;
        posy = e.clientY + document.body.scrollTop +
            document.documentElement.scrollTop;
    }
    posx = posx - offset.left;
    posy = posy - offset.top;

    return [posx,posy];
}

$('document').ready( function() {
    imagewidth  = 0;
    imageheight = 0;

    var img = new Image();
    img.src = path1;
    img.onload = function() {
        imagewidth  = img.width;
        imageheight = img.height;
        console.log(imagewidth, imageheight);

        column_width = Math.max(imageheight*1.5, 500);
        paper = Raphael(document.getElementById("canvas"), 2*column_width, imageheight);

        lastx1=0;
        lasty1=0;
        count1=0;

        lastx2=0;
        lasty2=0;
        count2=0;

        cornerx1=10;
        cornery=10;
        cornerx2= cornerx1+column_width;

        offset = ($("#canvas").offset());
        console.log(offset.left);
        console.log(offset.top);
        $("#seg1_cell").width(column_width);

        function draw1(e) {
            var pos = getPos(e);
            var posx = pos[0];
            var posy = pos[1];
            var ci=paper.circle(posx,posy, 5).attr({fill: "#000"});
            if (count1%2>0) {
                paper.path("M"+lastx1+" "+lasty1+" L"+posx+" "+posy);
                S = document.getElementById("seg1").innerHTML;
                S = S + writeCppSegment("segsBefore", lastx1-cornerx1, lasty1-cornery, posx-cornerx1, posy-cornery);
                document.getElementById("seg1").innerHTML=S;
            }
            lastx1 = posx;
            lasty1 = posy;
            count1 = count1+1;
        }

        function draw2(e) {
            var pos = getPos(e);
            var posx = pos[0];
            var posy = pos[1];
            var ci=paper.circle(posx,posy, 5).attr({fill: "#000"});
            if (count2%2>0){
                paper.path("M"+lastx2+" "+lasty2+" L"+posx+" "+posy);
                S = document.getElementById("seg2").innerHTML;
                S = S + writeCppSegment("segsAfter", lastx2-cornerx2, lasty2-cornery, posx-cornerx2, posy-cornery);
                document.getElementById("seg2").innerHTML=S;
            }
            lastx2=posx;
            lasty2=posy;
            count2=count2+1;
        }

        var bfg=paper.image(path1, cornerx1, cornery, imagewidth, imageheight);
        bfg.click(draw1);
        var bfg2=paper.image(path2, cornerx2, cornery, imagewidth, imageheight);
        bfg2.click(draw2);
    };
});
