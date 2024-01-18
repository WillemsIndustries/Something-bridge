function save(){
    wifi_SSID = document.getElementById("wifi_SSID");
    wifi_password = document.getElementById("wifi_password");
    HA_address = document.getElementById("HA_address");
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/save?wifi_SSID=" + wifi_SSID.value + 
    "&wifi_password=" + wifi_password.value + 
    "&HA_address=" + HA_address.value
    , true);
    xhttp.send();
}

function load_data(){
    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/load", true);
    xhttp.responseType = "text";
    xhttp.addEventListener("load", () => {
        let text = xhttp.responseText;
        text = text.slice(2,-2);
        const data = text.split('"},{"');
        var option = document.createElement("option");
        option.text = data[0];
        option.value = data[0];
        document.getElementById("wifi_SSID").add(option);
        document.getElementById("wifi_password").value = data[1];
        document.getElementById("HA_address").value = data[2];
        get_networks(data[0]);
    });
    xhttp.send();
}

function get_networks(saved_ssid){
    const xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/networks", true);
    xhttp.responseType = "text";
    xhttp.addEventListener("load", () => {
        let text = xhttp.responseText;
        text = text.slice(2,-2);
        const data = text.split('"},{"');
        for (let i = 0; i < data.length; i++) {
            if(data[i] != "" && data[i] != saved_ssid){
                var option = document.createElement("option");
                option.text = data[i];
                option.value = data[i];
                document.getElementById("wifi_SSID").add(option);
            }
        }
    });
    xhttp.send();
}