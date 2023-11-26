const canvas = document.getElementById('supermarket');
const ctxIconCustomer = canvas.getContext('2d');
const ctxIconCollition = canvas.getContext('2d');
const ctxSquare = canvas.getContext("2d");
const ctxProduct = canvas.getContext("2d");

const img = new Image();
img.src = "img/marker_client.png";
const imgCollition = new Image();
imgCollition.src = "img/red_alert.svg";

const DIM = 40;

let idInterval;
let tickets;
let customerColor = new Map();
let customerTickets = new Map();
let customerPickingPos = new Map();
let locationsShared;
let locationsCollition;
let locationsTotal = [];

let speedText = 1; // es podria multiplicar per 1000, 100, 10 o 1.
let speedValue = 1000;
let cw = canvas.width = container.offsetWidth;
let ch = canvas.height = container.offsetHeight;

let superMap = new Map();

document.getElementById('filePicker').addEventListener('change', readFile, false);
document.getElementById('filePickerMap').addEventListener('change', readFileMap, false);
document.getElementById('speed_text').innerHTML = "Velocidad Actual: " + speedText;

/**
 * Read CSV file 
 * @param e file content
 */
function readFile(e) {
    tickets = new Map();
    var file = e.target.files[0];
    if (!file) {
        return;
    }
    var reader = new FileReader();
    reader.onload = (eLoad) => {
        var data1 = eLoad.target.result;
        getDataOfFile(data1);
    };
    reader.readAsText(file);
}
function readFileMap(e) {
    var file = e.target.files[0];
    if (!file) {
        return;
    }
    var reader = new FileReader();
    reader.onload = (eLoad) => {
        var data2 = eLoad.target.result;
        getDataOfMap(data2);
    };
    reader.readAsText(file);
}

function getDataOfMap(contents)
{
    const breakLine = contents.split("\n");
    for (let i = 1; i < breakLine.length; i++) 
    {
        let [x, y, description, picking_x, picking_y] = breakLine[i].split(';');
        if(description == "paso" || description == "almacen" ||description == "escaleras" || description == "paso-entrada" || description == "paso-salida") continue;
        console.log(description)
        if (superMap.has(Number(x))) {
            const superMapValue = superMap.get(Number(x));
            superMapValue.add(Number(y));
            superMap.set(Number(x), superMapValue);
        }
        else 
            superMap.set(Number(x), new Set([Number(y)]));
    }
}

/**
 * Get content of the CSV file, line by line, splitting it by semicolon.
 * @param contents Content of the file
 */
function getDataOfFile(contents) {
    const dataCSV = []; //No se usa 
    const customerIdA = [];
    locationsTotal = [];
    let tabla = document.getElementById("tablaTickets");
    const breakLine = contents.split("\n");              //Array de las lineas del CSV
    const shopOpeningTime = caculateOpenDate(breakLine); //Hora de inicio del supermercado
    for (let i = 1; i < breakLine.length; i++) 
    {
        let [customer_id, ticket_id, x, y, picking, x_y_date_time] = breakLine[i].split(';');
        var index = dataCSV.findIndex((element) => element[0] === customer_id);
        if (index === -1) {
            const sec = epochConverter(x_y_date_time, shopOpeningTime)
            if (tickets.has(ticket_id)) {
                const locationsList = tickets.get(ticket_id);
                locationsList.push({ x, y, sec, ticket_id });
                tickets.set(ticket_id, locationsList);
            }
            else 
            {
                tickets.set(ticket_id, [{ x, y, sec, ticket_id }]);
                customerIdA.push(customer_id);
            }
            if (picking == 1 && customerPickingPos.has(ticket_id)) {
                let customerPos = customerPickingPos.get(ticket_id);
                if(!customerPos.some(obj => obj.x === x && obj.y === y))
                {
                    customerPos.push({x:x,y:y});
                    customerPickingPos.set(ticket_id, customerPos);
                }
            }
            else if (picking == 1) 
                customerPickingPos.set(ticket_id, [{x:x,y:y}]);
            if(ticket_id) locationsTotal.push({ x: x, y: y, s: sec, t: ticket_id });
        }

    }
    let it = 0;
    for (let [key, value] of tickets)
    {
        let fila = tabla.insertRow(-1);
        let column = fila.insertCell(0);
        column.innerHTML = "No Completado";
        column.style.color = "red";
        column.style.fontWeight = "bold";

        fila.insertCell(1).innerHTML = customerIdA[it];
        fila.insertCell(2).innerHTML = new Date(value[0].sec*1000+shopOpeningTime).toLocaleTimeString(navigator.language, {hour: '2-digit', minute:'2-digit', second: '2-digit'})
        fila.insertCell(3).innerHTML = new Date(value[value.length-1].sec*1000+shopOpeningTime).toLocaleTimeString(navigator.language, {hour: '2-digit', minute:'2-digit', second: '2-digit'});

        fila.insertCell(4).innerHTML = calcularDuracion(value[0].sec*1000, value[value.length-1].sec*1000);
        fila.insertCell(5).innerHTML = key;
        //fila.insertCell(6).innerHTML = "??";
   
        ctxSquare.clearRect(0,0,DIM,DIM);


        it = it + 1;
    }

    getSharedAndCollitionLocations();
    /*idInterval = setInterval(() => {
        let actTime = document.getElementById("timeId");
        actTime.innerHTML = sumarSegundos(actTime.innerHTML,1);
          },speedValue*100);*/
    calculateFirstcustomerSec();
}

function sumarSegundos(hora, segundosParaSumar) {
    var partes = hora.split(':');
    var fecha = new Date();

    fecha.setHours(partes[0], partes[1]);

    fecha.setMinutes(fecha.getMinutes() + segundosParaSumar);

    var horas = fecha.getHours().toString().padStart(2, '0');
    var minutos = fecha.getMinutes().toString().padStart(2, '0');

    return horas + ':' + minutos;
}


function calcularDuracion(tiempoInicial, tiempoFinal) {
    // Asegúrate de que los tiempos estén en el mismo formato (por ejemplo, milisegundos)
    var duracion = tiempoFinal - tiempoInicial;

    // Convertir la duración a segundos
    duracion = duracion / 1000;

    var horas = Math.floor(duracion / 3600);
    duracion = duracion - (horas * 3600);

    var minutos = Math.floor(duracion / 60);
    var segundos = duracion - (minutos * 60);

    return horas + " horas, " + minutos + " minutos, y " + segundos + " segundos.";
}

/**
 * Generates an object with locations and time of the CSV file
 * @param locations customer list of locations points 
 * @return waypoints - object of locations and time
 */
function calcWaypoints(locations) {
    var waypoints = [];
    var time = locations[0].sec;
    for (var i = 0; i < locations.length; i++) {
        var pt = locations[i];
        var dx = (pt.x - 1) * DIM;
        var dy = (pt.y - 1) * DIM;
        waypoints.push({ x: dx, y: dy, s: pt.sec, t: (time + i) });
    }
    return (waypoints);
}

/**
 *Calculates the first second of each ticket.
 */
async function calculateFirstcustomerSec() {
    var i = 0;
    for (let [key, value] of tickets) {
        const color = generateColor();
        customerTickets.set(color, key)
        customerColor.set(key, color)
        //console.log(value);
        value = sortRouteByTime(value); //No hace falta, lectura ordenada.
        console.log(+value[0].sec);
        const firstSecond = (+value[0].sec) * speedValue;
        const locations = calcWaypoints(value);
        await drawRouteAfterSeconds(locations, firstSecond, color, i, key);
        i = i + 1;
    }
}
/**
 * Execute drawRoute function after first second given by calculateFirstcustomerSec function
 * @param locations customer list of locations points 
 * @param firstSecond customer first second
 * @param color HEX color value
 */
function drawRouteAfterSeconds(locations, firstSecond, color, index, key) {
    setTimeout(() => {
        drawRoute(locations, color, index, key);
    }, firstSecond);
}
/**
 * Draw the customers route, with its image, and the color assigned to it. 
 * @param locationRoute customer list of locations points 
 * @param color HEX color value
 */
async function drawRoute(locationRoute, color, index, key) {
    const locRoute = locationRoute;
    let collition = false;
    let pos = [0,0];
    var tabla = document.getElementById("tablaTickets");
    var fila = tabla.rows[index+1];
    fila.cells[0].innerHTML = "En Ruta";
    fila.cells[0].style.color = "blue";
    for (let point_a of locRoute) {
        if(point_a.x != pos[0] || point_a.y != pos[1])
            drawSquare(point_a.x, point_a.y, color, 0.2);
        pos[0] = point_a.x;
        pos[1] = point_a.y;
    }
    let selectPosX = 0;
    let selectPosY = 0;
    let clean = false;
    for (let point of locRoute) {
        for (let loc of locationsCollition) {
            const x = ((+loc.split('U')[0]) - 1) * DIM
            const y = ((+loc.split('U')[1]) - 1) * DIM
            const s = ((+loc.split('U')[2]))
            if (point.s == s && point.x == x && point.y == y) {
                drawSquare(point.x, point.y, color);
                await sleep(speedValue);
                drawLocationsCollition(x, y)
                collition = true
            }
        }
        if (!collition) {
            if (point.s != point.t) {
                clearRoute(locRoute);
                return;
            }
            drawSquare(point.x, point.y, color,1);
            ctxIconCustomer.drawImage(img, point.x, point.y, DIM, DIM);
            let clean = false;
            if(customerPickingPos.has(key) && selectPosX != (point.x/DIM)+1 && selectPosY != (point.y/DIM)+1)
            {
                if(customerPickingPos.get(key).some(obj => obj.x == (point.x/DIM)+1 && obj.y == (point.y/DIM)+1))
                {
                    console.log("entrada picking");
                    if(existeEnMapa((point.x/DIM), (point.y/DIM)+1))
                        drawSquare(point.x-DIM, point.y, color,0.5);  
                    else if(existeEnMapa((point.x/DIM)+1, (point.y/DIM)))
                        drawSquare(point.x, point.y-DIM, color,0.5);
                    else if (existeEnMapa((point.x/DIM)+2, (point.y/DIM)+1))
                        drawSquare(point.x+DIM, point.y, color,0.5);
                    else 
                        drawSquare(point.x, point.y+DIM, color,0.5);
                    clean = true;
                }
            }
            await sleep(speedValue);
            if(clean && selectPosX != (point.x/DIM)+1 && selectPosY != (point.y/DIM)+1)
            {
                console.log("salida picking");
                console.log(existeEnMapa(26, 4));
                if(existeEnMapa((point.x/DIM), (point.y/DIM)+1))
                    clearSquare(point.x-DIM, point.y, DIM,DIM);
                else if(existeEnMapa((point.x/DIM)+1, (point.y/DIM)))
                    clearSquare(point.x, point.y-DIM, DIM,DIM);
                else if (existeEnMapa((point.x/DIM)+2, (point.y/DIM)+1))
                    clearSquare(point.x+DIM, point.y, DIM,DIM);
                else 
                    clearSquare(point.x, point.y+DIM, DIM,DIM);
                console.log(superMap);
            }
            drawSquare(point.x, point.y, color,1);
        } 
        collition = false
    }
    await sleep((speedValue / 2));
    fila.cells[0].innerHTML = "Completado";
    fila.cells[0].style.color = "green";
    clearRoute(locRoute, color);

}

function existeEnMapa(x, y) {
    if (superMap.has(Number(x))) {
        return superMap.get(Number(x)).has(Number(y));
    }
    return false;
}

/**
 * Deletes the points of the route, when the customer leaves the store 
 * @param locationRoute customer list of locations points 
 * @param color HEX color value
 * @return returns
 */
function clearRoute(locationRoute, color) {
    for (let point of locationRoute) { ctxSquare.clearRect(point.x, point.y, DIM, DIM); }
    const lastLocation = locationRoute[locationRoute.length - 1]
    const colorTicket = customerTickets.get(color);
    for (let [key, value] of locationsShared) {
        if (value.length == 0) continue
        value.delete(colorTicket);
        locationsShared.set(key, value);

    }
    for (let [key, value] of locationsShared) {
        for (let ticket of value) {
            if (lastLocation.s > (+key.split('U')[2])) {
                drawSquare(((+key.split('U')[0]) - 1) * DIM, ((+key.split('U')[1]) - 1) * DIM, customerColor.get(ticket), 1);
            }
        }
    }
    for (let loc of locationsCollition) {
        if (lastLocation.s > (+loc.split('U')[2])) {
            drawLocationsCollition(((+loc.split('U')[0]) - 1) * DIM, ((+loc.split('U')[1]) - 1) * DIM);
        }
    }

}

function hexToRgba(hex, alpha) {
    let r = parseInt(hex.slice(1, 3), 16),
        g = parseInt(hex.slice(3, 5), 16),
        b = parseInt(hex.slice(5, 7), 16);

    return `rgba(${r}, ${g}, ${b}, ${alpha})`;
}

/**
 * Function for draw square with specific color
 * @param x x location 
 * @param y y location
 * @param color HEX color value
 */
function drawSquare(x, y, hexColor, alpha) {
    let color = hexToRgba(hexColor, alpha);
    ctxSquare.lineCap = 'square';
    ctxSquare.fillStyle = color;
    ctxSquare.fillRect(x, y, DIM, DIM);
}

function clearSquare(x, y, width, height) {
    ctxSquare.clearRect(x, y, width, height);
}

/**
 * Generates a new different color.
 * @return randomColor
 */
function generateColor() {
    let maxVal = 0xFFFFFF;
    let randomColor;
    do {
        randomColor = Math.floor(Math.random() * maxVal);
    } while (randomColor === 0xFFFFFF);
    let randColor = randomColor.toString(16).padStart(6, "0");
    return `#${randColor.toUpperCase()}`
}
/**
 * Waits ms seconds
 * @param ms miliseconds
 */
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
/**
 * Function called from html buttons, that modifies the speed of the route. 
 * Also modifies the text shown between the buttons. 
 * @param buttonAction passing add or remove depending on which button is clicked. 
 */
function modifySpeed(buttonAction) {
    
    if (buttonAction == 'add' && speedText < 4) {         
        speedText = speedText + 1;
        speedValue = speedValue/10 ;
    } else if(buttonAction=='remove' && speedText >1) {
        speedText = speedText - 1;
        speedValue = speedValue*10; 
    }
    document.getElementById('speed_text').innerHTML = "Velocitat Actual: " + speedText;
}


/**
 *Converts date and time from the CSV file to epoctime and returns milliseconds.
 * @param date day and time format like "yyyy-mm-dd hh:mm:ss"
 * @param shopOpeningTime epochtime of the shop open time.
 * @return milliseconds
 */
function epochConverter(date, shopOpeningTime) {
    var epochtime = Date.parse(date);
    return ((epochtime - shopOpeningTime) / 1000);
}

/**
 * Sort arrayValues by time and returns it.
 * @param arrayValues array of locations and time
 * @return arrayValues
 */
function sortRouteByTime(arrayValues) {
    for (var i = 0; i < arrayValues.length; i++) {
        for (var j = i + 1; j < arrayValues.length; j++) {
            if (arrayValues[i].sec > arrayValues[j].sec) {
                let lineValueAux = arrayValues[i];
                arrayValues[i] = arrayValues[j];
                arrayValues[j] = lineValueAux;
            }
        }
    }
    return (arrayValues);
}

/**
 * Read the date (day) of the breakLine, set the time to 9:00 and returns it in epochtime.
 * @param breakLine line from CSV file
 * @return epochtime of the shop opening time
 */
function caculateOpenDate(breakLine) {
    let [a, b, c, d, e, date] = breakLine[1].split(';');
    let datestr = date.substring(0, 10);
    let timestr = "09:00:00";
    datestr = datestr.concat(" ", timestr);
    return Date.parse(datestr);
}
/**
 * Function that draws all the collitions.
 */
function drawLocationsCollition(x, y) {
    ctxIconCollition.drawImage(imgCollition, x, y, DIM, DIM);
}
/**
 * Function to set a Shared locations list 
 * and collition location list.
 */
function getSharedAndCollitionLocations() {
    locationsShared = new Map();
    locationsCollition = [];
    locationsTotal.forEach((element, index) => {
        locationsTotal.forEach((loc, i) => {
            const idList = loc.x.concat('U').concat(loc.y).concat('U').concat(loc.s);
            if (element.x == loc.x && element.y == loc.y && element.s == loc.s && i != index) {
                if (locationsCollition.indexOf(idList) == -1) locationsCollition.push(idList)
            }
            if (element.x == loc.x && element.y == loc.y && element.t != loc.t && i != index) {
                const idList = loc.x.concat('U').concat(loc.y).concat('U').concat(loc.s);
                if (locationsShared.has(idList)) {
                    const newList = locationsShared.get(idList);
                    newList.add(loc.t);
                    locationsShared.set(idList, new Set(newList));
                }
                else { locationsShared.set(idList, new Set([loc.t])); }
            }
        });
    });
}