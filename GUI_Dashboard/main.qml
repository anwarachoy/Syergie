import QtQuick 2.0

Rectangle {
    id: dashboard

    visible: true
    width: 1366
    height: 768
    color: "#0b0b0b"

    function updateValue() {
        //Enggine 1
        propelerDirection1.angle = mqttvalue.azimut1()
        speedEngine1.text = mqttvalue.enginespeed1()
        speedPropeler1.text = mqttvalue.propelerspeed1()
        fuelNeedle1.rotation = mqttvalue.fuel1()
        tempNeedle1.rotation = mqttvalue.temp1()
        depth1.text = mqttvalue.vdepth1()
        indicator1.color = mqttvalue.engineconect1()

        //Enggine 2
        propelerDirection2.angle = mqttvalue.azimut2()
        speedEngine2.text = mqttvalue.enginespeed2()
        speedPropeler2.text = mqttvalue.propelerspeed2()
        fuelNeedle2.rotation = mqttvalue.fuel2()
        tempNeedle2.rotation = mqttvalue.temp2()
        depth2.text = mqttvalue.vdepth2()
        indicator2.color = mqttvalue.engineconect2()

        //Enggine 3
        propelerDirection3.angle = mqttvalue.azimut3()
        speedEngine3.text = mqttvalue.enginespeed3()
        speedPropeler3.text = mqttvalue.propelerspeed3()
        fuelNeedle3.rotation = mqttvalue.fuel3()
        tempNeedle3.rotation = mqttvalue.temp3()
        depth3.text = mqttvalue.vdepth3()
        indicator3.color = mqttvalue.engineconect3()

        //Enggine 4
        propelerDirection4.angle = mqttvalue.azimut4()
        speedEngine4.text = mqttvalue.enginespeed4()
        speedPropeler4.text = mqttvalue.propelerspeed4()
        fuelNeedle4.rotation = mqttvalue.fuel4()
        tempNeedle4.rotation = mqttvalue.temp4()
        depth4.text = mqttvalue.vdepth4()
        indicator4.color = mqttvalue.engineconect4()

        //WindCondition
        windDirection.angle = mqttvalue.directWind()
        speedWind.text = mqttvalue.speedWind()
        indicator5.color = mqttvalue.windconect()
		
		mqttvalue.maincontrol()
    }

	function reMap(value, maxInput, minInput, maxOutput, minOutput){

        //value = maxInput if value > maxInput else value
        //value = minInput if value < minInput else value

		inputSpan = maxInput - minInput
		outputSpan = maxOutput - minOutput

        //scaledThrust =  (value - minInput) / float(inputSpan)

		return minOutput + (scaledThrust * outputSpan)
	}


    Item {
        id: container
        x: 0
        y: 0
        width: 1366
        height: 768

        Image {
            id: background
            x: 0
            y: 0
            source: "MockUpFix.png"


            Item {
                id: firstEngine
                x: 16
                y: 23
                width: 480
                height: 340

                Image {
                    id: tempNeedle1
                    x: 315
                    y: 72
                    width: 8
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"
                    Image {
                        id: o
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                Image {
                    id: fuelNeedle1
                    x: 347
                    y: 72
                    rotation: reMap
                    transformOrigin: Item.Bottom
                    source: "2.png"

                    Image {
                        id: o1
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                TextEdit {
                    id: speedEngine1
                    x: 102
                    y: 160
                    width: 45
                    height: 20
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignRight
                    font.wordSpacing: -0.2
                    renderType: Text.NativeRendering
                    font.pointSize: 11; font.bold: true
                }

                TextEdit {
                    id: speedPropeler1
                    x: 102
                    y: 295
                    width: 45
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Image {
                    id: propelerDirectionNeedle1
                    x: 324
                    y: 184
                    width: 22
                    height: 66
                    source: "3.png"
                    transform: [
                        Rotation {
                            id: propelerDirection1
                            origin.x: 11
                            origin.y: 49
                            angle: 0
                        }]

                    Image {
                        id: o2
                        x: 6
                        y: 44
                        source: "4.png"
                    }

                }

                Image {
                    id: engineNeedle1
                    x: 136
                    y: 67
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "1.png"

                    Image {
                        id: bigO1
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                Image {
                    id: proprelerNeedle1
                    x: 136
                    y: 202
                    rotation: 0
                    source: "1.png"
                    transformOrigin: Item.Bottom
                    Image {
                        id: bigO2
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                TextEdit {
                    id: depth1
                    x: 308
                    y: 130
                    width: 57
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Rectangle {
                    id: indicator1
                    x: 420
                    y: 30
                    width: 30
                    height: 30
                    color: "#ffffff"
                    radius: 15
                    border.width: 0
                }
            }


            Item {
                id: secondEngine
                x: 874
                y: 23
                width: 480
                height: 340

                Image {
                    id: tempNeedle2
                    x: 315
                    y: 72
                    width: 8
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"
                    Image {
                        id: o3
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                Image {
                    id: fuelNeedle2
                    x: 347
                    y: 72
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"

                    Image {
                        id: o4
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                TextEdit {
                    id: speedEngine2
                    x: 102
                    y: 160
                    width: 45
                    height: 20
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignRight
                    font.wordSpacing: -0.2
                    renderType: Text.NativeRendering
                    font.pointSize: 11; font.bold: true
                }

                TextEdit {
                    id: speedPropeler2
                    x: 102
                    y: 295
                    width: 45
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Image {
                    id: propelerDirectionNeedle2
                    x: 324
                    y: 184
                    width: 22
                    height: 66
                    transform: [
                        Rotation {
                            id: propelerDirection2
                            angle: 0
                            origin.y: 49
                            origin.x: 11
                        }]
                    source: "3.png"
                    Image {
                        id: o5
                        x: 6
                        y: 43
                        source: "4.png"
                    }
                }

                Image {
                    id: engineNeedle2
                    x: 136
                    y: 67
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "1.png"

                    Image {
                        id: bigO3
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                Image {
                    id: proprelerNeedle2
                    x: 136
                    y: 202
                    rotation: 0
                    source: "1.png"
                    transformOrigin: Item.Bottom
                    Image {
                        id: bigO4
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                TextEdit {
                    id: depth2
                    x: 308
                    y: 130
                    width: 57
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Rectangle {
                    id: indicator2
                    x: 420
                    y: 30
                    width: 30
                    height: 30
                    color: "#ffffff"
                    radius: 15
                    border.width: 0
                }
            }

            Item {
                id: thirdEngine
                x: 16
                y: 407
                width: 480
                height: 340

                Image {
                    id: tempNeedle3
                    x: 315
                    y: 72
                    width: 8
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"
                    Image {
                        id: o6
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                Image {
                    id: fuelNeedle3
                    x: 347
                    y: 72
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"

                    Image {
                        id: o7
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                TextEdit {
                    id: speedEngine3
                    x: 102
                    y: 160
                    width: 45
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                TextEdit {
                    id: speedPropeler3
                    x: 102
                    y: 296
                    width: 45
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Image {
                    id: propelerDirectionNeedle3
                    x: 324
                    y: 184
                    width: 22
                    height: 66

                    transform: [
                        Rotation {
                            id: propelerDirection3
                            angle:0
                            origin.y: 49
                            origin.x: 11
                        }]
                    source: "3.png"
                    Image {
                        id: o8
                        x: 6
                        y: 44
                        source: "4.png"
                    }
                }

                Image {
                    id: engineNeedle3
                    x: 136
                    y: 67
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "1.png"

                    Image {
                        id: bigO5
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                Image {
                    id: proprelerNeedle3
                    x: 136
                    y: 202
                    rotation: 0
                    source: "1.png"
                    transformOrigin: Item.Bottom
                    Image {
                        id: bigO6
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                TextEdit {
                    id: depth3
                    x: 308
                    y: 130
                    width: 57
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Rectangle {
                    id: indicator3
                    x: 420
                    y: 30
                    width: 30
                    height: 30
                    color: "#ffffff"
                    radius: 15
                    border.width: 0
                }
            }

            Item {
                id: fourthEngine
                x: 874
                y: 407
                width: 480
                height: 340

                Image {
                    id: tempNeedle4
                    x: 315
                    y: 72
                    width: 8
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"
                    Image {
                        id: o9
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                Image {
                    id: fuelNeedle4
                    x: 347
                    y: 72
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "2.png"

                    Image {
                        id: o10
                        x: -1
                        y: 33
                        source: "4.png"
                    }
                }

                TextEdit {
                    id: speedEngine4
                    x: 102
                    y: 160
                    width: 45
                    height: 20
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignRight
                    font.wordSpacing: -0.2
                    renderType: Text.NativeRendering
                    font.pointSize: 11; font.bold: true
                }

                TextEdit {
                    id: speedPropeler4
                    x: 102
                    y: 295
                    width: 45
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Image {
                    id: propelerDirectionNeedle4
                    x: 324
                    y: 185
                    width: 22
                    height: 66
                    transform: [
                        Rotation {
                            id: propelerDirection4
                            angle: 0
                            origin.y: 48
                            origin.x: 11
                        }]
                    source: "3.png"
                    Image {
                        id: o11
                        x: 6
                        y: 43
                        source: "4.png"
                    }
                }

                Image {
                    id: engineNeedle4
                    x: 136
                    y: 67
                    rotation: 0
                    transformOrigin: Item.Bottom
                    source: "1.png"

                    Image {
                        id: bigO7
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                Image {
                    id: proprelerNeedle4
                    x: 136
                    y: 202
                    rotation: 0
                    source: "1.png"
                    transformOrigin: Item.Bottom
                    Image {
                        id: bigO8
                        x: -2
                        y: 66
                        source: "bigO.png"
                    }
                }

                TextEdit {
                    id: depth4
                    x: 308
                    y: 130
                    width: 57
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 11
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Rectangle {
                    id: indicator4

                    x: 420
                    y: 30
                    width: 30
                    height: 30
                    color: "#ffffff"
                    radius: 15
                    border.width: 0
                }
            }

            Item {
                id: windCondition
                x: 517
                y: 217
                width: 336
                height: 336

                Image {
                    id: windNeedle
                    x: 160
                    y: 101
                    width: 22
                    height: 66
                    transform: [
                        Rotation {
                            id: windDirection
                            angle: 0
                            origin.y: 49
                            origin.x: 11
                        }]
                    source: "3.png"
                    Image {
                        id: o12
                        x: 6
                        y: 44
                        source: "4.png"
                    }
                }


                TextEdit {
                    id: directionWind
                    x: 199
                    y: 242
                    width: 30
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 10
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                    text : windDirection.angle
                }
                TextEdit {
                    id: speedWind
                    x: 169
                    y: 272
                    width: 45
                    height: 20
                    color: "#ffffff"
                    font.pointSize: 10
                    font.bold: true
                    renderType: Text.NativeRendering
                    font.wordSpacing: -0.2
                    horizontalAlignment: Text.AlignRight
                }

                Rectangle {
                    id: indicator5
                    x: 270
                    y: 36
                    width: 30
                    height: 30
                    color: "#ffffff"
                    radius: 15
                    border.width: 0
                }
            }

        }
        Rectangle {
            id: exitButton
            x: 634
            width: 100; height: 40
            color: "#e90000"
            border.color: "#00000000"
            y: 636
            Text {
                id: buttonexit
                color: "#ffffff"
                text: qsTr("Quit")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 24; font.bold: true
            }
            MouseArea {
                id: butMouseArea
                anchors.top: exitButton.bottom
                anchors.right: exitButton.left
                anchors.bottom: exitButton.top
                anchors.left: exitButton.right
                hoverEnabled: false
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                onClicked: 	Qt.quit();
            }
        }
    }
}
