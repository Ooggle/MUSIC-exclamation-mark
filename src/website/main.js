var audio;
var obj;

window.onload = function() {

    if ('mediaSession' in navigator) {
        navigator.mediaSession.metadata = new MediaMetadata({
            title: "TITLE",
            artist: "ARTIST",
            album: "ALBUM",
            artwork: [{
                sizes: "320x180",// <- MUST BE EXACTLY!
                src: "https://i.ytimg.com/vi/yAruCvT7P7Y/hqdefault.jpg?sqp=-oaymwEZCNACELwBSFXyq4qpAwsIARUAAIhCGAFwAQ==&rs=AOn4CLAfHWw5BHrQugGsdPYy4eIXcqMTnQ",
                type: ""
            }]
        });
    
        navigator.mediaSession.setActionHandler('play', function () { });
        navigator.mediaSession.setActionHandler('pause', function () { });
        navigator.mediaSession.setActionHandler('seekbackward', function () { });
        navigator.mediaSession.setActionHandler('seekforward', function () { });
        navigator.mediaSession.setActionHandler('previoustrack', function () { });
        navigator.mediaSession.setActionHandler('nexttrack', function () { });
    }

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            //document.getElementById("demo").innerHTML = this.responseText;
            obj = JSON.parse(this.responseText);

            for(key in obj.musics) {
                var divParent = document.createElement("div");
                var imageCover = document.createElement("img");
                var firstPar = document.createElement("p");
                var secondPar = document.createElement("p");
                
                divParent.setAttribute("music-id", obj.musics[key].id);
                divParent.addEventListener("click", function(){
                    playMusic(this.getAttribute("music-id"));
                });

                imageCover.src = "album_cover.jpg";
                firstPar.appendChild(document.createTextNode(obj.musics[key].artist + " - " + obj.musics[key].title));
                secondPar.appendChild(document.createTextNode(obj.musics[key].album + " (" + obj.musics[key].year + ")"));

                var fragment = new DocumentFragment();

                fragment.appendChild(imageCover);
                fragment.appendChild(firstPar);
                fragment.appendChild(secondPar);

                divParent.appendChild(fragment);

                var parent = document.getElementById("music-container");
                parent.appendChild(divParent);

                //console.log(obj.musics[key]);
            }
        } else if(this.readyState == 4 && this.status != 200) {
            document.getElementById("music-container").innerHTML = "No connexion with server.";
        }
    };
    xhttp.open("GET", "http://192.168.0.44:81/music-db?id=1", true);
    xhttp.send();
}

function playMusic(id) {
    console.log(id);
    document.getElementById("current-playing-title").innerHTML = obj.musics[id - 1].title;

    audio = document.getElementById('yourAudioTag');
    audio.src = "http://192.168.0.44:81/music?id=" + id;
    audio.play();
    audio.volume = 0.4;

    audio.onended = function() {
        alert("The audio has ended");
    };
}

function getdur() {
    audio.currentTime = 25;
}