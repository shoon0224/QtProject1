function func(lati, longi){
    let kmaWeather = require('kma-js').Weather;
    kmaWeather.townWeather(lati, longi)
    .then(data => console.log(data));
}
