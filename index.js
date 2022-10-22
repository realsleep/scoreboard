const express = require('express')
const bodyParser = require('body-parser')
const app = express()
const port = 80

let team_number;
let team_names;
let matches = [];

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
    res.send(matches);
})

app.post('/team_names', (req, res) => {
    team_names = req.body;
    const matches_number = team_number / 2;
    for (let i = 1; i <= matches_number; i++) {
        console.log(i);
        matches[i-1] = [team_names["team_name" + (2 * i - 1)], team_names["team_name" + 2 * i]]
    }
    res.redirect("/form_matches.html");
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})