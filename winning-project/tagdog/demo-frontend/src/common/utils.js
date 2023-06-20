export function setCookie(cname, cvalue, exminutes) {
    var d = new Date();
    d.setTime(d.getTime + (exminutes*60*1000));
    var expires = "expires=" + d.toUTCString();
    document.cookie = cname + "=" + cvalue + ";" + expires + ";path=/";
}