# SERVER API

## Music request

endpoint: /music

method: GET

parameters:
- id: int

response: a direct link to the music file

<br>

## Music database request

endpoint: /music-db

method: GET

parameters:
- id: int

response: a json file

```json
{
    "musics": [
        {
            "album_id": int,
            "artist": string,
            "comment": string,
            "filename": string,
            "genre": string,
            "id": int,
            "title": string,
            "track_number": int,
            "year": int
            // strings can be unicode o/ kanjis
        },
        {...}
    ]
}
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
    "message": string,
    "result_code": int
    // result code 0 = success
}
```

<br>

## Get user informations

endpoint: /get-user-infos

method: GET

parameters:
- username: string

response: a json file

```json
{
    "result_code": int,
    // result code 0 = success
    "informations": [
        "id": int,
        "directories": [
            string,
            string,
            {...}
            // can be empty
        ],
        "library_revision": int,
        "creation_date": string
    ]
}
```

<br>
