var current_page = 0;
var max_page = 0;
var arr=[];
var play_state = 0;
var buffer = "";
var buffer = [];
var exist = 0;

var slider = document.getElementById("tempobar");
slider.oninput = function() {
  document.getElementById('tempo').innerHTML = "Tempo : " + this.value;
}

function init(){
    for(var i=0; i<8; i++){
        for(var j=0; j<8; j++){
            document.getElementById('b'+i+j).style.background = "white";
        }
    }
    for(var i=0; i<4; i++){
        arr[i] = [];
        for(var j=0; j<8; j++){
            arr[i][j] = []
            for(var k=0; k<8; k++){
                arr[i][j][k] = 0;
            }
        }
    }
    for(var i=0; i<5; i++){
        buffer[i] = "";
    }
    console.log('initialized');
    console.log(arr);
}
init();
function boxclicked(num){
    var A = parseInt(num/10);
    var B = num%10;
    if(arr[current_page][A][B] == 0){
        arr[current_page][A][B] = 1;
        document.getElementById('b'+A+B).style.background = document.getElementsByClassName('row'+A)[0].style.background;
        clearCol(A,B);
    }
    else if(arr[current_page][A][B] == 1){
        arr[current_page][A][B] = 0;
        document.getElementById('b'+A+B).style.background = "white";
    }
    console.log(num);
    console.log('b'+parseInt(num/10)+num%10)
    console.log('row'+parseInt(num/10))
}

function showinfo(){
    console.log('Max page : '+max_page)
    console.log('Current page : '+current_page)
}

function refresh(){
    for(var A=0; A<8; A++){
        for(var B=0; B<8; B++){
            if(arr[current_page][A][B] == 1){
                document.getElementById('b'+A+B).style.background = document.getElementsByClassName('row'+A)[0].style.background;
            }
            else if(arr[current_page][A][B] == 0){
                document.getElementById('b'+A+B).style.background = "white";
            }
        }
    }
    document.getElementById('info').innerHTML = "Current Page : " + (current_page+1) + "<br>Max Page : " + (max_page+1);
}

function playmusic(){
    if(exist == 0){
        exist = 1;
        play_state = 1;
        document.getElementById('play').innerHTML = "Stop";
        tempo = document.getElementById('tempobar').value;
        console.log(tempo)
        buffer[0] = (180-tempo) + 'X';
        for(var i=0; i<4; i++){
            if(i<max_page+1){
                for(var k=0; k<8; k++){
                    for(var j=0; j<8; j++){
                        if(arr[i][j][k] == 1){
                            if(j == 0){
                                buffer[i+1] = buffer[i+1] + 'C';
                                break;
                            }
                            else if(j == 1){
                                buffer[i+1] = buffer[i+1] + 'b';
                                break;
                            }
                            else if(j == 2){
                                buffer[i+1] = buffer[i+1] + 'a';
                                break;
                            }
                            else if(j == 3){
                                buffer[i+1] = buffer[i+1] + 'g';
                                break;
                            }
                            else if(j == 4){
                                buffer[i+1] = buffer[i+1] + 'f';
                                break;
                            }
                            else if(j == 5){
                                buffer[i+1] = buffer[i+1] + 'e';
                                break;
                            }
                            else if(j == 6){
                                buffer[i+1] = buffer[i+1] + 'd';
                                break;
                            }
                            else{
                                buffer[i+1] = buffer[i+1] + 'c';
                                break;
                            }
                        }
                        if(j == 7){
                            buffer[i+1] = buffer[i+1] + 'R';
                        }
                    }
                }
            }
            else{
                buffer[i+1] = buffer[i+1] + 'ZZZZZZZZ';
            }
            buffer[i+1] = buffer[i+1] + 'X';
            console.log('='+buffer[i+1])
        }
    }
    else{
        if(play_state == 0){
            play_state = 1;
            document.getElementById('play').innerHTML = "Stop";
            buffer[0] = 'P';
            buffer[1] = 'P';
            buffer[2] = 'P';
            buffer[3] = 'P';
            buffer[4] = 'P';
        }
        else{
            play_state = 0;
            document.getElementById('play').innerHTML = "Play";
            buffer[0] = 'S';
            buffer[1] = 'S';
            buffer[2] = 'S';
            buffer[3] = 'S';
            buffer[4] = 'S';
        }
    }
}

function pluspage(){
    if(max_page<3){
        max_page++;
    }
    refresh();
    showinfo();
}

function minuspage(){
    if(max_page > 0){
        max_page--;
    }
    if(max_page == current_page-1){
        current_page = max_page;
    }
    refresh();
    showinfo();
}

function nextpage(){
    if(current_page < max_page){
        current_page++;
    }
    refresh();
    showinfo();
}

function prevpage(){
    if(current_page > 0){
        current_page--;
    }
    refresh();
    showinfo();
}


function clearCol(num1, num2){
    for(var i=0; i<8; i++){
        if(i!=num1){
            arr[current_page][i][num2] = 0;
        }
    }
    refresh();
}

function rst(){
    exist = 0;
    play_state = 0;
    document.getElementById('play').innerHTML = "Play";
    for(var i=0; i<4; i++){
        for(var j=0; j<8; j++){
            for(var k=0; k<8; k++){
                arr[i][j][k] = 0;
                console.log('arr['+i+j+k+']')
            }
        }
    }

    for(var A=0; A<8; A++){
        for(var B=0; B<8; B++){
            document.getElementById('b'+A+B).style.background = "white";
        }
    }
    for(var i=0; i<5; i++){
        buffer[i] = "";
    }
    max_page = 0;
    current_page = 0;
    refresh();
    showinfo();
    console.log('rst done')
}

$('#play').click((event)=>{
event.preventDefault()
    playmusic();
    $.get('play',{
      //send buffer to server.js
      bfr0 : buffer[0],
      bfr1 : buffer[1],
      bfr2 : buffer[2],
      bfr3 : buffer[3],
      bfr4 : buffer[4],
    },(data)=>{
    });
});

$('#rst').click((event)=>{
event.preventDefault()
    rst();
    $.get('rst',{
    },(data)=>{
    });

});
