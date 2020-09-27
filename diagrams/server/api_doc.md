# SERVER API

## Music request

endpoint: /music

method: GET

parameters:
- id: integer

response: a direct link to the music file

<br>

## Music database request

endpoint: /music-db

method: GET

parameters:
- id: integer

response: a json file

```json
{
    "musics": [
        {
            "album_id":  "integer",
            "artist":  "string",
            "comment":  "string",
            "filename":  "string",
            "genre":  "string",
            "id":  "integer",
            "title":  "string",
            "track_number":  "integer",
            "year":  "integer"
        },
        {"..."}
    ]
}
```
```
strings can be unicode, o/ kanjis
```

<br>

## Create user

endpoint: /create-user

method: GET

parameters:
- username: string
- password: string

response: a json file

```json
{
    "message":  "string",
    "result_code":  "integer"
}
```
```
"result_code": 0 = success
```

<br>

## Get user informations

endpoint: /get-user-infos

method: GET

parameters:
- username:  "string"

response: a json file

```json
{
    "result_code":  "integer",
    "informations": [
        "id":  "integer",
        "directories": [
             "string",
             "string",
            {...}
        ],
        "library_revision":  "integer",
        "creation_date":  "string"
    ]
}
```
```
"result_code": 0 = success
"directories" node can be empty or not present
```

<br>
