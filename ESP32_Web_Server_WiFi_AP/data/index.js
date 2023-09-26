const add_game_btn = document.getElementById("add_game_btn");
const cancel_btn = document.getElementById("cancel_btn");
const save_btn = document.getElementById("save_btn");
const name1 = document.getElementById("name1");
const name2 = document.getElementById("name2");
const num_matches = document.getElementById("num_matches");
const dur_match = document.getElementById("dur_match");
let card_number = 0;

// Get the modal
const modal = document.getElementById("myModal");

// When the user clicks on the button, open the modal
add_game_btn.onclick = function () {
  modal.style.display = "flex";
};

save_btn.onclick = function () {
  event_handler_add_game_btn(
    name1.value,
    name2.value,
    num_matches.value,
    dur_match.value
  );
  name1.value = null;
  name2.value = null;
  num_matches.value = null;
  dur_match.value = null;
  modal.style.display = "none";
};

// When the user clicks anywhere outside of the modal, close it
cancel_btn.onclick = function (event) {
  modal.style.display = "none";
};

async function load() {
  const response = await fetch(`/match/all`);
  const jsonData = await response.json();
  console.log(jsonData);
}

async function event_handler_add_game_btn(
  name1_text,
  name2_text,
  num_matches_text,
  dur_match_text
) {
  const card_holder = document.getElementById("card_holder");

  const div = document.createElement("div");
  div.className = "col-md-4";
  div.id = "card-" + card_number;
  const card = document.createElement("div");
  card.className = "card p-3 mb-2";

  card.appendChild(name_and(name1_text, name2_text));
  card.appendChild(badge("Match", num_matches_text));
  card.appendChild(badge("Duration", dur_match_text));

  div.appendChild(card);

  card_holder.appendChild(div);
  card_number++;

  const response = await fetch(
    `/match/add?name1=${name1_text}&name2=${name2_text}&round_dur=${dur_match_text}& rounds_num=${num_matches_text}`
  );
  const jsonData = await response.json();
  console.log(jsonData);
}

function name_and(name1, name2, scores) {
  const div = document.createElement("div");
  div.className = "row";
  div.appendChild(create_name(name1));
  const button = document.createElement("button");
  button.type = "button";
  button.className = "play btn btn-primary btn-lg";
  button.innerHTML = "PLAY";
  button.id = "play-" + card_number;
  button.onclick = play;
  div.appendChild(button);
  div.appendChild(create_name(name2));
  return div;
}

function create_name(text) {
  const name = document.createElement("p");
  name.className = "col text-center h5";
  name.innerHTML = text;
  return name;
}

function create_scores(text) {
  const scores = document.createElement("p");
  scores.className = "col text-center my-auto h1";
  scores.innerHTML = text;
  return scores;
}

function badge(text, data) {
  const div = document.createElement("div");
  div.className = "row-mb-1 mx-auto";
  const cont = document.createElement("div");
  cont.className = "badge badge-primary bg-primary";
  const badge_label = document.createElement("span");
  badge_label.innerHTML = text + ": ";
  const badge_value = document.createElement("span");
  badge_value.innerHTML = data;
  cont.appendChild(badge_label);
  cont.appendChild(badge_value);
  div.appendChild(cont);
  return div;
}

function play(e) {
  const id = e.target.id;
  const index = id[id.length - 1];
  const card_rows = document.getElementById("card-" + index).childNodes[0]
    .childNodes;
  const first_row = card_rows[0];
  const second_row = card_rows[1];
  const third_row = card_rows[2];

  const name1 = first_row.childNodes[0].innerHTML;
  const name2 = first_row.childNodes[2].innerHTML;
  const match_num = second_row.childNodes[0].childNodes[1].innerHTML;
  const match_dur = third_row.childNodes[0].childNodes[1].innerHTML;

  // Simulate an HTTP redirect:
  window.location.replace(
    `game.html?id=${id}&team1=${name1}&team2=${name2}&match_num=${match_num}`
  );
}
load();
