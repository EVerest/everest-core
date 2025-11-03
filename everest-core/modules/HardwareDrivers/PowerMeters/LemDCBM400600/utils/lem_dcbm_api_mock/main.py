from datetime import datetime, timezone
from pathlib import Path
from multiprocessing import Process

import uvicorn
from fastapi import FastAPI, APIRouter
from fastapi.responses import PlainTextResponse
from pydantic import BaseModel, Field

app = FastAPI()

v1_api = APIRouter()

class UTCTimeSetting(BaseModel):
    utc: str

class TimeSetting(BaseModel):
    time: UTCTimeSetting

class LEMSyncTimePutSettingsResponse(BaseModel):
    meterId: str
    result: int

@v1_api.put("/settings")
def put_settings(body: TimeSetting) -> LEMSyncTimePutSettingsResponse:
    return LEMSyncTimePutSettingsResponse(**{"meterId":"mock_meter_id","result":1})


class LEMLiveMeasure(BaseModel):
    voltage: float
    current: float
    power: float
    temperatureH: float
    temperatureL: float
    energyImportTotal: float
    energyExportTotal: float
    timestamp: datetime  # in ISO 8601 format


@v1_api.get("/livemeasure")
def livemeasure() -> LEMLiveMeasure:
    return LEMLiveMeasure(**{"voltage": 0,
                             "current": 0,
                             "power": 0,
                             "temperatureH": 0,
                             "temperatureL": 0,
                             "energyImportTotal": 0,
                             "energyExportTotal": 0,
                             "timestamp": datetime.now(timezone.utc)}
                          )


class LEMStartTransactionRequest(BaseModel):
    evseId: str
    transactionId: str
    clientId: str
    tariffId: int
    cableId: int
    userData: str


class LEMStartTransactionPostResponse(BaseModel):
    evseId: str
    transactionId: str
    clientId: str
    tariffId: int
    cableId: int
    running: bool


@v1_api.post("/legal", status_code=201)
def start_transaction(body: LEMStartTransactionRequest) -> LEMStartTransactionPostResponse:
    return LEMStartTransactionPostResponse(**
                                           {"evseId": "evse458877",
                                            "transactionId": body.transactionId,
                                            "clientId": "client12",
                                            "tariffId": 2, "cableId": 2,
                                            "running": True}
                                           )


class LEMPutTransactionRequestCableSP(BaseModel):
    cableSpName: str
    cableSpId: int
    cableSpRes: int


class LEMPutTransactionRequestValue(BaseModel):
    energyUnit: str
    energyImport: float
    energyImportTotalStart: float
    energyImportTotalStop: float
    energyExport: float
    energyExportTotalStart: float
    energyExportTotalStop: float


class LEMPutTransactionRequestMeterValue(BaseModel):
    timestampStart: datetime
    timestampStop: datetime
    transactionDuration: int
    intermediateRead: bool
    transactionStatus: int  # note: error in first description p. 45
    sampleValue: LEMPutTransactionRequestValue


class LEMPutTransactionRequestResponse(BaseModel):
    paginationCounter: int
    transactionId: str
    evseId: str
    clientId: str
    tariffId: int
    cableSp: LEMPutTransactionRequestCableSP
    userData: str
    meterValue: LEMPutTransactionRequestMeterValue

    meterId: str
    signature: str
    publicKey: str


class LEMPutTransactionRequestBody(BaseModel):
    running: bool


@v1_api.put("/legal")
def put_transaction(transactionId: str, body: LEMPutTransactionRequestBody) -> LEMPutTransactionRequestResponse:
    return LEMPutTransactionRequestResponse(
        **{
            "paginationCounter": 6,
            "transactionId": transactionId,
            "evseId": "+49*DEF*E123ABC",
            "clientId": "C12",
            "tariffId": 2,
            "cableSp": {
                "cableSpName": "2mR_Comp",
                "cableSpId": 1,
                "cableSpRes": 2
            },
            "userData": "",
            "meterValue": {
                "timestampStart": "2020-12-10T16:39:15+01:00",
                "timestampStop": "2020-12-10T16:39:15+01:00",
                "transactionDuration": 70,
                "intermediateRead": False,
                "transactionStatus": 25,
                "sampleValue": {
                    "energyUnit": "kWh",
                    "energyImport": 7.637,
                    "energyImportTotalStart": 188.977,
                    "energyImportTotalStop": 196.614,
                    "energyExport": 0.000,
                    "energyExportTotalStart": 0.000,
                    "energyExportTotalStop": 0.000
                }},
            "meterId": "12024072805",
            "signature": "304502203DC38FBC722D216568D6ECB4B3"
                         "52577A999B6D184EA6AD48BDCAE7766DB1D628022100A768"
                         "7B4CB5573829D407DD4B17D41C297917B7E8307E5017711B"
                         "5A3A987F6801",
            "publicKey": "A80F10D968E1122F8820F288B23C4E1C0D"
                         "A912F35B48481274ADFEFE66D7E87E130C7CF2B8047C45CF"
                         "105041C8C3A57DD242782F755C9443F42DABA9404A67BF"
        }
    )


class OCMFReading(BaseModel):
    TM: str  # actually datetime + status
    TX: str
    RV: float
    RI: str
    RU: str
    RT: str
    EF: str
    ST: str
    UC: dict  # not in standard? LEM : "This field reflects the /settings/cableConf selected table for the transaction by the /legal/ cableId input parameter. This is a LEM specific field, using specific IDs:"


class OCMFPart1(BaseModel):
    FV: str
    GI: str
    GS: str
    GV: str
    PG: str
    MV: str
    MM: str
    MS: str
    MF: str
    IS: bool
    IL: str
    IF: list[str]
    IT: str
    ID: str
    TT: str
    RD: list[OCMFReading]


OCMF_EXAMPLE_JSON1 = {
    "FV": "1.0",
    "GI": "ABL SBC-301",
    "GS": "808829900001",
    "GV": "1.4p3",
    "PG": "T12345",
    "MV": "Phoenix Contact",
    "MM": "EEM-350-D-MCB",
    "MS": "BQ27400330016",
    "MF": "1.0",
    "IS": True,
    "IL": "VERIFIED",
    "IF": [
        "RFID_PLAIN",
        "OCPP_RS_TLS"
    ],
    "IT": "ISO14443",
    "ID": "1F2D3A4F5506C7",
    "TT": "Tarif 1",
    "RD": [
        {
            "TM": "2018-07-24T13:22:04,000+0200 S",
            "TX": "B",
            "RV": 2935.6,
            "RI": "1-b:1.8.0",
            "RU": "kWh",
            "RT": "AC",
            "EF": "",
            "ST": "G",
            # LEM Special
            "UC": {"UN": "cableName", "UI": 1, "UR": 1}
        }
    ]
}


class OCMFPart2(BaseModel):
    SA: str = Field(
        required=False)  # NOTE: could not find this in https://github.com/SAFE-eV/OCMF-Open-Charge-Metering-Format/blob/master/OCMF-en.md, but specified in LEM
    SD: str


OCMF_EXAMPLE_JSON2 = {
    "SA": "ECDSA-secp256r1-SHA256",  # LEM special
    "SD": "887FABF407AC82782EEFFF2220C2F856AEB0BC22364BBCC6B55761911ED651D1A922BADA88818C9671AFEE7094D7F536"
}


@v1_api.get("/ocmf", response_class=PlainTextResponse)
def get_last_transaction_ocmf_by_transaction_id(transactionId: str | None) -> str:
    return f"OCMF|{OCMFPart1(**OCMF_EXAMPLE_JSON1)}|{OCMFPart2(**OCMF_EXAMPLE_JSON2)}"


@v1_api.get("/ocmf/{transactionIndex}", response_class=PlainTextResponse)
def get_last_transaction_ocmf_by_transaction_index(transactionIndex: int) -> str:
    return f"OCMF|{OCMFPart1(**OCMF_EXAMPLE_JSON1)}|{OCMFPart2(**OCMF_EXAMPLE_JSON2)}"


class LemDCBMStatusBits(BaseModel):
    suLinkStatusIsOk: bool
    muFatalErrorOccured: bool
    transactionIsOnGoing: bool
    tamperingIsDetected: bool
    timeSyncStatusIsOk: bool
    overTemperatureIsDetected: bool
    reversedVoltage: bool
    suMeasureFailureOccurred: bool


class LemDCBMStatusVersion(BaseModel):
    applicationFirmwareVersion: str
    applicationFirmwareAuthTag: str
    legalFirmwareVersion: str
    legalFirmwareAuthTag: str
    sensorFirmwareVersion: str
    sensorFirmwareCrc: str


class LemDCBMStatusErrorsBits(BaseModel):
    muInitIsFailed: bool
    suStateIsInvalid: bool
    versionCheckIsFailed: bool
    muRngInitIsFailed: bool
    muDataIntegrityIsFailed: bool
    muFwIntegrityIsFailed: bool
    suIntegrityIsFailed: bool
    logbookIntegrityIsFailed: bool
    logbookIsFull: bool
    memoryAccessIsFailed: bool
    muStateIsFailed: bool


class LemDCBMStatusErrors(BaseModel):
    value: int
    bits: LemDCBMStatusErrorsBits


class LemDCBMStatus(BaseModel):
    value: int
    bits: LemDCBMStatusBits


class LemDCBMStatusResponse(BaseModel):
    status: LemDCBMStatus
    version: LemDCBMStatusVersion
    time: str
    ipAddress: str
    meterId: str
    errors: LemDCBMStatusErrors
    publicKey: str
    publicKeyOcmf: str
    indexOfLastTransaction: int
    numberOfStoredTransactions: int


@v1_api.get("/status")
def get_status() -> LemDCBMStatusResponse:
    return LemDCBMStatusResponse(**{"status": {
        "value": 17,
        "bits": {
            "suLinkStatusIsOk": True,
            "muFatalErrorOccured": True,
            "transactionIsOnGoing": True,
            "tamperingIsDetected": True,
            "timeSyncStatusIsOk": True,
            "overTemperatureIsDetected": True,
            "reversedVoltage": True,
            "suMeasureFailureOccurred": True
        }},
        "version": {
            "applicationFirmwareVersion": "string",
            "applicationFirmwareAuthTag": "string",
            "legalFirmwareVersion": "string",
            "legalFirmwareAuthTag": "string",
            "sensorFirmwareVersion": "string",
            "sensorFirmwareCrc": "string"
        },
        "time": "string",
        "ipAddress": "string",
        "meterId": "mock_meter_id",
        "errors": {
            "value": 0,
            "bits": {
                "muInitIsFailed": False,
                "suStateIsInvalid": False,
                "versionCheckIsFailed": False,
                "muRngInitIsFailed": False,
                "muDataIntegrityIsFailed": False,
                "muFwIntegrityIsFailed": False,
                "suIntegrityIsFailed": False,
                "logbookIntegrityIsFailed": False,
                "logbookIsFull": False,
                "memoryAccessIsFailed": False,
                "muStateIsFailed": False,
            }
        },
        "publicKey": "string",
        "publicKeyOcmf": "string",
        "indexOfLastTransaction": 0,
        "numberOfStoredTransactions": 99
    })

app.include_router(v1_api, prefix="/v1")

def run_http_api():
    uvicorn.run("main:app",
                host="0.0.0.0",
                port=8000, reload=True)

def run_https_api():
    uvicorn.run("main:app",
                host="0.0.0.0",
                port=8443, 
                reload=True,
                ssl_keyfile=str(Path(__file__).parent / "./key.pem"),
                ssl_certfile=str(Path(__file__).parent / "./certificate.pem"))

if __name__ == "__main__":
    p_http = Process(target=run_http_api)
    p_https = Process(target=run_https_api)
    p_http.start()
    p_https.start()
    p_http.join()
    p_https.join()

