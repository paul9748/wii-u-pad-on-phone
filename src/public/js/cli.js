const socket = io();

const scale = 1;
window.ondevicemotion = function (motion) {
    //x(red) z(blue) y(green) 
    var gyroV = {
        x: scale * motion.rotationRate.alpha,
        z: scale * motion.rotationRate.beta,
        y: -scale * motion.rotationRate.gamma,
    }
    var gyroH = {
        z: scale * motion.rotationRate.alpha,
        x: -scale * motion.rotationRate.beta,
        y: -scale * motion.rotationRate.gamma,
    }

    document.getElementById("x").textContent = 'x:' + gyroV.x
    document.getElementById("y").textContent = 'y:' + gyroV.y
    document.getElementById("z").textContent = 'z:' + gyroV.z
    var data = {
        ts: new Date().getTime(),
        gyro: screen == 'v' ? gyroV : gyroH,
        acceleration: {
            x: 0,
            y: 0,
            z: 0
        }
    }
    console.log(gyroV);
};