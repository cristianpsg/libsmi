//The scripts for the tooltip and moveobj are based on work from
//SVG - Learning By Coding - http://www.datenverdrahten.de/svglbc/
//Author: Dr. Thomas Meinike 11/03 - thomas@handmadecode.de
var svgdoc,svgroot;
var collapsed = new Array(2);
var name = new Array(%i);
var clickStatus = new Array(%i);
var redCount = new Array(%i);
var salmonCount = new Array(%i);
var moveObj,x,y,attr1,attr2,wert1,wert2,zoom=1,active=false;

function MoveObj(evt)
{
    if(active)
    {
	var curtrans=svgroot.currentTranslate;
	var ctx=curtrans.x;
	var cty=curtrans.y;

	x=evt.clientX()
	y=evt.clientY()

	wert1=(x-ctx)*zoom*%.2f+%.2f+attr1-5;
	wert2=(y-cty)*zoom*%.2f+%.2f+attr2-5;

	moveObj.setAttribute("transform","translate("+wert1+","+wert2+")");
    }
}

function ClickObj(evt)
{
    moveObj=evt.target.parentNode.parentNode;
    attr1=evt.target.getAttribute("width")*%.2f/2;
    attr2=evt.target.getAttribute("height")*%.2f/2;
    active=true;
}

function OutOfObj(evt)
{
    active=false;
}

function getSVGDoc(load_evt)
{
    svgdoc=load_evt.target.ownerDocument;
    svgroot=svgdoc.documentElement;
    texte=svgdoc.getElementById("tooltip").getElementsByTagName("text");
    for (i=1; i<%i; i++) {
	textNode=texte.item(1).cloneNode("true");
	svgdoc.getElementById("tooltip").appendChild(textNode);
    }
    texte=svgdoc.getElementById("tooltip").getElementsByTagName("text");
}

function ShowTooltipMZ(mousemove_event,txt)
{
    var ttrelem,tttelem,ttline,posx,posy,curtrans,ctx,cty,txt;
    var maxbreite,tmp,i;
    ttrelem=svgdoc.getElementById("ttr");
    tttelem=svgdoc.getElementById("ttt");
    ttline=svgdoc.getElementById("ttl");
    posx=mousemove_event.clientX;
    posy=mousemove_event.clientY;
    for (i=1;i<=%i;i++)
	texte.item(i).firstChild.data="";
    tttelem.childNodes.item(0).data="";
    tmp=txt.split("\n");
    maxbreite=0;
    for (i=0;i<tmp.length;i++) {
	if (tmp[i]=="")
	    continue;
	tttelem.childNodes.item(0).data=tmp[i];
	if (maxbreite<tttelem.getComputedTextLength())
	    maxbreite=tttelem.getComputedTextLength();
    }
    curtrans=svgroot.currentTranslate;
    ctx=curtrans.x;
    cty=curtrans.y;
    ttrelem.setAttribute("x",posx-ctx+10);
    ttrelem.setAttribute("y",posy-cty-20+10);
    ttrelem.setAttribute("width",maxbreite*0.92+10);
    ttrelem.setAttribute("height",tmp.length*15+3);
    ttrelem.setAttribute("style",
			    "fill: #FFC; stroke: #000; stroke-width: 0.5px");
    for (i=1; i<=tmp.length; i++) {
	if (tmp[i-1]=="-- -- --") {
	    ttline.setAttribute("x1", posx-ctx+10);
	    ttline.setAttribute("y1", parseInt(i-1)*15+posy-cty);
	    ttline.setAttribute("x2", posx-ctx+10+maxbreite*0.92+10);
	    ttline.setAttribute("y2", parseInt(i-1)*15+posy-cty);
	    ttline.setAttribute("style", "stroke: #000; stroke-width: 0.5px");
	} else {
	    texte.item(i).firstChild.data=tmp[i-1];
	    texte.item(i).setAttribute("x",posx-ctx+15);
	    texte.item(i).setAttribute("y",parseInt(i-1)*15+posy-cty+3);
	    texte.item(i).setAttribute("style","fill: #00E; font-size: 11px");
	}
    }
    svgdoc.getElementById("tooltip").style.setProperty("visibility","visible");
}

function HideTooltip()
{
    svgdoc.getElementById("ttl").style.setProperty("visibility","hidden");
    svgdoc.getElementById("tooltip").style.setProperty("visibility","hidden");
}

function ZoomControl()
{
    var curzoom;
    curzoom=svgroot.currentScale;
    svgdoc.getElementById("tooltip").setAttribute("transform",
							"scale("+1/curzoom+")");
    zoom=1/curzoom;
}

function collapse(evt)
{
    var i, k, l, m, svgdoc, obj, targetID, targetX, targetY, attr;
    obj = evt.getTarget();
    svgdoc = obj.ownerDocument;
    //extract coordinates and id of the clicked text
    attr = obj.parentNode.parentNode.attributes;
    for (i=0;i<attr.length;i++) {
        if (attr.item(i).nodeName == "transform") {
            k = attr.item(i).nodeValue.indexOf("(");
            l = attr.item(i).nodeValue.indexOf(",");
            m = attr.item(i).nodeValue.indexOf(")");
            targetX = attr.item(i).nodeValue.substring(k+1,l);
            targetY = attr.item(i).nodeValue.substring(l+1,m);
        }
        if (attr.item(i).nodeName == "id") {
            targetID = attr.item(i).nodeValue.substr(2);
        }
    }
    //decide if we are collapsing or uncollapsing
    if (collapsed[0][targetID] == 0) {
        hideInfos(evt, obj, svgdoc, targetX, targetY, targetID, attr);
        collapsed[0][targetID] = 1;
    } else {
        showHiddenInfos(evt, obj, svgdoc, targetX, targetY, targetID, attr);
        collapsed[0][targetID] = 0;
    }
}

function showHiddenInfos(evt, obj, svgdoc, targetX, targetY, targetID, attr)
{
    var i, k, l, m, nextObj, nextX, nextY, gapY=0, clickedID;
    //change clicked text
    obj.firstChild.data="--";
    targetID++;
    nextObj = svgdoc.getElementById("MI"+targetID);
    if (nextObj == null)
        return;
    //show child texts again
    for (;;) {
        attr = nextObj.attributes;
        for (i=0;i<attr.length;i++) {
            if (attr.item(i).nodeName == "transform") {
                k = attr.item(i).nodeValue.indexOf("(");
                l = attr.item(i).nodeValue.indexOf(",");
                m = attr.item(i).nodeValue.indexOf(")");
                nextX = attr.item(i).nodeValue.substring(k+1,l);
                nextY = attr.item(i).nodeValue.substring(l+1,m);
            }
        }
        if (nextX > targetX) {
            nextObj.style.setProperty("visibility","visible");
            gapY += 15;
            if (collapsed[0][targetID] == 1) {
                targetID = collapsed[1][targetID];
            } else {
                targetID++;
            }
            nextObj = svgdoc.getElementById("MI"+targetID);
            if (nextObj == null)
                break;
        } else {
            break;
        }
    }
    if (nextObj == null)
        return;
    //move following texts downwards
    while (nextObj != null) {
        attr = nextObj.attributes;
        for (i=0;i<attr.length;i++) {
            if (attr.item(i).nodeName == "transform") {
                k = attr.item(i).nodeValue.indexOf("(");
                l = attr.item(i).nodeValue.indexOf(",");
                m = attr.item(i).nodeValue.indexOf(")");
                nextX = attr.item(i).nodeValue.substring(k+1,l);
                nextY = attr.item(i).nodeValue.substring(l+1,m);
            }
        }
        nextY = nextY - 1 + gapY + 1;
        nextObj.setAttribute("transform","translate("+nextX+","+nextY+")");
        targetID++;
        nextObj = svgdoc.getElementById("MI"+targetID);
    }
}

function hideInfos(evt, obj, svgdoc, targetX, targetY, targetID, attr)
{
    var i, k, l, m, nextObj, nextX, nextY, gapY=0;
    clickedID = targetID;
    //change clicked text
    obj.firstChild.data="+";
    targetID++;
    nextObj = svgdoc.getElementById("MI"+targetID);
    if (nextObj == null)
        return;
    //wipe out child texts
    for (;;) {
        attr = nextObj.attributes;
        for (i=0;i<attr.length;i++) {
            if (attr.item(i).nodeName == "transform") {
                k = attr.item(i).nodeValue.indexOf("(");
                l = attr.item(i).nodeValue.indexOf(",");
                m = attr.item(i).nodeValue.indexOf(")");
                nextX = attr.item(i).nodeValue.substring(k+1,l);
                nextY = attr.item(i).nodeValue.substring(l+1,m);
            }
        }
        if (nextX > targetX) {
            nextObj.style.setProperty("visibility","hidden");
            gapY += 15;
            if (collapsed[0][targetID] == 1) {
                targetID = collapsed[1][targetID];
            } else {
                targetID++;
            }
            nextObj = svgdoc.getElementById("MI"+targetID);
            if (nextObj == null)
                break;
        } else {
            break;
        }
    }
    //save next uncollapsed element in array
    collapsed[1][clickedID] = targetID;
    if (nextObj == null)
        return;
    //move following texts upwards
    while (nextObj != null) {
        attr = nextObj.attributes;
        for (i=0;i<attr.length;i++) {
            if (attr.item(i).nodeName == "transform") {
                k = attr.item(i).nodeValue.indexOf("(");
                l = attr.item(i).nodeValue.indexOf(",");
                m = attr.item(i).nodeValue.indexOf(")");
                nextX = attr.item(i).nodeValue.substring(k+1,l);
                nextY = attr.item(i).nodeValue.substring(l+1,m);
            }
        }
        nextY -= gapY;
        nextObj.setAttribute("transform","translate("+nextX+","+nextY+")");
        targetID++;
        nextObj = svgdoc.getElementById("MI"+targetID);
    }
}

function init(evt)
{
    collapsed[0] = new Array(%i);
    collapsed[1] = new Array(%i);
    for (i=0; i<%i; i++) {
        collapsed[0][i] = 0;
        collapsed[1][i] = 0;
    }
    for (i=0; i<%i; i++) {
	name[i] = "";
	clickStatus[i] = 0;
	redCount[i] = 0;
	salmonCount[i] = 0;
    }
    getSVGDoc(evt);
}

function setStatus(evt, color1, color2)
{
    var clickObj = evt.getTarget();
    var clickObjName = clickObj.getAttribute('id');

    //find i corresponding to the clicked object
    for (i=0; i<%i; i++) {
	if (name[i] == "") {
	    name[i] = clickObjName;
	    break;
	}
	if (name[i] != clickObjName)
	    continue;
	break;
    }

    //toggle click status, color clicked object
    if (clickStatus[i] == 0) {
	clickStatus[i] = 1;
	clickObj.setAttribute("style","fill: "+color1);
    } else {
	clickStatus[i] = 0;
	clickObj.setAttribute("style","fill: "+color2);
    }

    //adjust color-counter
    if (color1 == 'red') {
	if (clickStatus[i] == 1) {
	    redCount[i]++;
	} else {
	    redCount[i]--;
	}
    }
    if (color1 == 'salmon') {
	if (clickStatus[i] == 1) {
	    salmonCount[i]++;
	} else {
	    salmonCount[i]--;
	}
    }

    if (clickStatus[i] == 0 && salmonCount[i] > 0) {
	clickObj.setAttribute("style","fill: salmon");
    }
}

function changeColor(evt, targetObjName, color1, color2)
{
    var clickObj = evt.getTarget();
    var clickObjName = clickObj.getAttribute('id');
    var targetObj = svgDocument.getElementById(targetObjName);

    //find i corresponding to the clicked object
    for (i=0; i<%i; i++) {
	if (name[i] != clickObjName)
	    continue;
	break;
    }

    //find j corresponding to the target object
    for (j=0; j<%i; j++) {
	if (name[j] == "") {
	    name[j] = targetObjName;
	    break;
	}
	if (name[j] != targetObjName)
	    continue;
	break;
    }

    //adjust color-counter
    if (color1 == 'red') {
	if (clickStatus[i] == 1) {
	    redCount[j]++;
	} else {
	    redCount[j]--;
	}
    }
    if (color1 == 'salmon') {
	if (clickStatus[i] == 1) {
	    salmonCount[j]++;
	} else {
	    salmonCount[j]--;
	}
    }
}

function colorText(targetObjName, color)
{
    var targetObj = svgDocument.getElementById(targetObjName);

    //find i corresponding to the target object
    for (i=0; i<%i; i++) {
	if (name[i] != targetObjName)
	    continue;
	break;
    }

    //color text
    if (i == %i) {
	targetObj.setAttribute("style","fill: "+color);
	return;
    }
    if (redCount[i] == 0 && salmonCount[i] == 0) {
	targetObj.setAttribute("style","fill: "+color);
    }
    if (salmonCount[i] > 0) {
	if (color == 'red') {
	    targetObj.setAttribute("style","fill: red");
	} else {
	    if (redCount[i] > 0) {
		targetObj.setAttribute("style","fill: red");
	    } else {
		targetObj.setAttribute("style","fill: salmon");
	    }
	}
    }
}