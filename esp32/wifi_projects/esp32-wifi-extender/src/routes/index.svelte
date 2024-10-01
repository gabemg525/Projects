<script>
    let ap_ssid = ''; // Placeholder for new network SSID
    let ap_password = ''; // Placeholder for new network Password
    let ssid = ''; // Placeholder for uplink network SSID
    let password = ''; // Placeholder for uplink network Password
    let ent_username = ''; // WPA2 Enterprise username
    let ent_identity = ''; // WPA2 Enterprise identity
    let staticip = ''; // Static IP
    let subnetmask = ''; // Subnet Mask
    let gateway = ''; // Gateway

    // Handle form submission logic here
    function handleSubmitConfig() {
        console.log("AP and STA settings submitted");
        // Add logic to send these inputs to your ESP32 server
    }
</script>

<style>
    body {
        font-family: apercu-pro, -apple-system, system-ui, BlinkMacSystemFont, 'Helvetica Neue', sans-serif;
        padding: 1em;
        line-height: 2em;
        font-weight: 100;
    }

    td {
        font-weight: 100;
        min-height: 24px;
    }

    td:first-child {
        text-align: right;
        min-width: 100px;
        padding-right: 10px;
    }

    h1 {
        font-size: 1.5em;
        font-weight: 200;
    }

    h2 {
        font-size: 1.2em;
        font-weight: 200;
        margin-left: 5px;
    }

    input {
        border: 1px solid rgb(196, 196, 196);
        color: rgb(76, 76, 76);
        width: 240px;
        border-radius: 3px;
        height: 40px;
        margin: 3px 0px;
        padding: 0px 14px;
    }

    input:focus {
        border: 1px solid black;
        outline: none !important;
        box-shadow: 0 0 10px #719ECE;
    }

    .ok-button {
        background-color: #0078e7;
        color: white;
    }

    .red-button {
        background-color: #e72e00;
        color: white;
    }

    #config {
        width: 400px;
        margin: 0 auto;
    }
</style>

<div id="config">
    <h1>ESP32 NAT Router Config</h1>
    
    <h2>AP Settings (the new network)</h2>
    <form on:submit|preventDefault={handleSubmitConfig}>
        <table>
            <tr>
                <td>SSID</td>
                <td><input type="text" bind:value={ap_ssid} placeholder="SSID of the new network" /></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="text" bind:value={ap_password} placeholder="Password of the new network" /></td>
            </tr>
            <tr>
                <td></td>
                <td><button type="submit" class="ok-button">Set</button></td>
            </tr>
        </table>
        <small>
            <i>Password </i> less than 8 chars = open<br />
        </small>
    </form>

    <h2>STA Settings (uplink WiFi network)</h2>
    <form on:submit|preventDefault={handleSubmitConfig}>
        <table>
            <tr>
                <td>SSID</td>
                <td><input type="text" bind:value={ssid} placeholder="SSID of existing network" /></td>
            </tr>
            <tr>
                <td>Password</td>
                <td><input type="text" bind:value={password} placeholder="Password of existing network" /></td>
            </tr>
            <tr>
                <td colspan="2">WPA2 Enterprise settings. Leave blank for regular</td>
            </tr>
            <tr>
                <td>Enterprise username</td>
                <td><input type="text" bind:value={ent_username} placeholder="WPA2 Enterprise username" /></td>
            </tr>
            <tr>
                <td>Enterprise identity</td>
                <td><input type="text" bind:value={ent_identity} placeholder="WPA2 Enterprise identity" /></td>
            </tr>
            <tr>
                <td></td>
                <td><button type="submit" class="ok-button">Connect</button></td>
            </tr>
        </table>
    </form>

    <h2>STA Static IP Settings</h2>
    <form on:submit|preventDefault={handleSubmitConfig}>
        <table>
            <tr>
                <td>Static IP</td>
                <td><input type="text" bind:value={staticip} /></td>
            </tr>
            <tr>
                <td>Subnet Mask</td>
                <td><input type="text" bind:value={subnetmask} /></td>
            </tr>
            <tr>
                <td>Gateway</td>
                <td><input type="text" bind:value={gateway} /></td>
            </tr>
            <tr>
                <td></td>
                <td><button type="submit" class="ok-button">Connect</button></td>
            </tr>
        </table>
        <small>
            <i>Leave it blank if you want your ESP32 to get an IP using DHCP</i>
        </small>
    </form>

    <h2>Device Management</h2>
    <form on:submit|preventDefault={handleSubmitConfig}>
        <table>
            <tr>
                <td>Device</td>
                <td><button type="submit" class="red-button">Reboot</button></td>
            </tr>
        </table>
    </form>
</div>

