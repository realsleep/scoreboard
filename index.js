const express = require('express')
const bodyParser = require('body-parser')
const app = express()
const port = 80

let team_number;
let team_names;
let current_match_id = -1;
let current_match_score = [0,0];
let current_match_team_name = [];
let matches = [];
let totalSeconds = 0;
let timerVariable;

app.use(express.static('./'))
app.use(bodyParser.urlencoded({
    extended: true
  }));


app.get('/', (req, res) => {
    res.redirect('team_number.html')
})

app.get('/team_number', (req, res) => {
    res.send(team_number)
})

app.post('/team_number', (req, res) => {
    team_number = req.body["team_number"]
    res.redirect("/team_names.html")
})

app.get('/team_names', (req, res) => {
    res.send(matches)
})

app.post('/team_names', (req, res) => {
    team_names = req.body
    const matches_number = team_number / 2
    for (let i = 1; i <= matches_number; i++) {
        matches[i-1] = [team_names["team_name" + (2 * i - 1)], team_names["team_name" + 2 * i]]
    }
    res.redirect("/form_matches.html");
})

app.get('/match/:ID', (req, res) => {
    current_match_id = req.params.ID
    current_match_team_name = matches[+current_match_id - 1]
    res.redirect("/match.html")
    timerVariable = setInterval(() => totalSeconds++, 1000);
})

app.get('/match/:TEAM_ID/score', (req, res) => {
    const TEAM_ID = req.params.TEAM_ID;
    res.send(current_match_score[+TEAM_ID - 1] + "")
})

app.get('/match/:TEAM_ID/score/add', (req, res) => {
    const TEAM_ID = req.params.TEAM_ID;
    current_match_score[+TEAM_ID - 1] = current_match_score[+TEAM_ID - 1] + 1
    res.send(current_match_score[+TEAM_ID - 1] + "")
})

app.get('/match/:TEAM_ID/score/sub', (req, res) => {
    const TEAM_ID = req.params.TEAM_ID;
    current_match_score[+TEAM_ID - 1] = current_match_score[+TEAM_ID - 1] - 1
    res.send(current_match_score[+TEAM_ID - 1] + "")
})


app.get('/match/:TEAM_ID/name', (req, res) => {
    const TEAM_ID = req.params.TEAM_ID;
    res.send(current_match_team_name[+TEAM_ID - 1])
})

app.get('/time', (req, res) => {
    res.send(totalSeconds + "");
})

app.get('/stop', (req, res) => {
    res.send(totalSeconds + "");
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})