function getString() {
    var S = "CorrespondencePair corresp[4] = {<br />";
    var maxpointsN = Math.max(points1N, points2N);

    for (var i = 0; i < maxpointsN; i++) {
        S = S + "CorrespondencePair( ";
        if (i < points1N) {
            S += points1[i];
        }
        S += ", ";
        if (i < points2N) {
            S += points2[i];
        }
        S += ")";
        if (i != maxpointsN-1) {
            S += ", <br />";
        }
    }
    S += "<br />};";
    return S;
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
    var img1 = new Image();
    img1.src = path1;
    imagewidth1  = 0;
    imageheight1 = 0;

    var img2 = new Image();
    img2.src = path2;
    imagewidth2  = 0;
    imageheight2 = 0;

    var ok1 = false;
    var ok2 = false;

    img1.onload = function() {
        imagewidth1  = img1.width;
        imageheight1 = img1.height;
        console.log(imagewidth1, imageheight1);
        ok1 = true;
        process();

    };

    img2.onload = function() {
        imagewidth2  = img2.width;
        imageheight2 = img2.height;
        console.log(imagewidth2, imageheight2);
        ok2 = true;
        process();

    };

    function process(){
        if(!ok1 || !ok2){
            return;
        }
        space = 10;
        column_width = imagewidth1+space;
        paper = Raphael(document.getElementById("canvas"), space+imagewidth1+imagewidth2, Math.max(imageheight1, imageheight2));


        cornerx1=10;
        cornery=10;
        cornerx2= cornerx1+column_width;

        points1 = [''];
        points1N = 0;
        points2 = [''];
        points2N = 0;

        offset = ($("#canvas").offset());
        console.log(offset.left);
        console.log(offset.top);

        function draw1(e) {
            var pos = getPos(e);
            var posx = pos[0];
            var posy = pos[1];
            var ci=paper.circle(posx,posy, 5).attr({fill: "#000"});

            points1[points1N] = ''+(posx-cornerx1)+","+(posy-cornery)+",1";
            points1N++;
            document.getElementById("points").innerHTML=getString();
            console.log(getString());
        }

        function draw2(e) {
            var pos = getPos(e);
            var posx = pos[0];
            var posy = pos[1];
            var ci=paper.circle(posx,posy, 5).attr({fill: "#000"});

            points2[points2N] = ''+(posx-cornerx2)+","+(posy-cornery)+",1";
            points2N++;
            document.getElementById("points").innerHTML=getString();
            console.log(getString());
        }

        var bfg=paper.image(path1, cornerx1, cornery, imagewidth1, imageheight1);
        bfg.click(draw1);

        var bfg2=paper.image(path2, cornerx2, cornery, imagewidth2, imageheight2);
        bfg2.click(draw2);
    }
});
