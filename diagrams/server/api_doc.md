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
    "code": "integer",
    "musics": [
        {
            "album_id": "integer",
            "artist": "string",
            "comment": "string",
            "filename": "string",
            "genre": "string",
            "id": "integer",
            "title": "string",
            "track_number": "integer",
            "year": "integer"
        },
        {"..."}
    ]
}
```
```
strings can be unicode, o/ kanjis
```

<br>

## Album database request

endpoint: /album-db

method: GET

parameters:
- id: integer

response: a json file

```json
{
    "code": "integer",
    "albums": [
        {
            "artist_id": "integer",
            "artist_name": "string",
            "id": "integer",
            "img": "string",
            "name": "string",
            "year": "integer"
        },
        {"..."}
    ]
}
```
```
strings can be unicode, o/ kanjis
```

<br>

<br>

## Artist database request

endpoint: /artist-db

method: GET

parameters:
- id: integer

response: a json file

```json
{
    "code": "integer",
    "artists": [
        {
            "id": "integer",
            "img": "string",
            "name": "string"
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
    "message": "string",
    "code": "integer"
}
```
```
"code": 0 = success
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
    "code": "integer",
    "informations": [
        {
        "id": "integer",
        "library_revision": "integer",
        "creation_date": "string"
        }
    ]
}
```
```
"code": 0 = success
```

<br>
