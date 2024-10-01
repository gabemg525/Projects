<script>
    let unlock_password = ''; // Store password input

    function handleUnlock() {
        console.log("Unlock submitted");
        // Add logic to handle unlocking
    }
</script>

<style>
    /* Use the same CSS as before */
</style>

<div id="config">
    <h1>ESP32 NAT Router Config</h1>
    <h2>Config Locked</h2>
    <form on:submit|preventDefault={handleUnlock}>
        <table>
            <tr>
                <td>Password:</td>
                <td><input type="password" bind:value={unlock_password} /></td>
            </tr>
            <tr>
                <td></td>
                <td><button type="submit" class="red-button">Unlock</button></td>
            </tr>
        </table>
        <small>
            <i>Default: STA password to unlock</i><br />
        </small>
    </form>
</div>

