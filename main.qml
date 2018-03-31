import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import leds 1.0
import "QMLFontAwesome" as FA

ApplicationWindow {
    id: window
    visible: true
    width: 900
    height: 680
    title: qsTr("LED Studio")

    readonly property int num_tracks: 4
    readonly property int timeline_cell_width: 50
    property int currentTimeTick: 0

    property int selectedTick: -1
    property int selectedTimeline: 0
    property int selectedGroupId: 0
    property int nextGroupId: 1
    property bool selectedTickUpdateNotifier: false

    FA.Icon {
        id: fa
        font.pointSize: 18
    }
    FA.Icon {
        id: fasmall
        font.pointSize: 8
    }

    ListModel {
        id: timelineModel
    }
    ListModel {
        id: ledTableModel
    }
    ListModel {
        id: colorsTableModel
    }

    Component {
        id: listModelComponent
        ListModel {
        }
    }

    function timelineAsJson() {
        var item;

        // Get all effect instances from timeline
        var effects = {};
        var track;
        for (track=0; track<4; track++) {
            effects[track] = [];
        }
        for (var tick=0; tick<timelineModel.count; tick++) {
            var timelineEntry = timelineModel.get(tick);
            for (track=0; track<4; track++) {
                item = timelineEntry.tracks.get(track);
                if (item.active && item.first) {
                    var effect = Object.assign({}, item.effect);
                    effect.props = Object.assign({}, item.props);
                    effect._ticks = timelineEntry.ticks; // needed to reload the data in the editor
                    effect._duration = item.duration; // needed to reload the data in the editor
                    effect.start_time = timelineEntry.ticks * 0.1;
                    effect.end_time = (timelineEntry.ticks + item.duration) * 0.1;
                    effects[track].push(effect);
                }
            }
        }

        // Get led tables
        var led_tables = {};
        for (var led_table=0; led_table<ledTableModel.count; led_table++) {
            item = ledTableModel.get(led_table);
            led_tables[item.name] = JSON.parse("["+item.leds+"]");
        }

        // Get color tables
        var color_tables = {};
        for (var color_table=0; color_table<colorsTableModel.count; color_table++) {
            item = colorsTableModel.get(color_table);
            var colors = [];
            for (var idx=0; idx<item.colors.count; idx++) {
                colors.push(item.colors.get(idx));
            }
            color_tables[item.name] = colors;
        }

        // Pack into required structure and jsonify
        var data = {
            tracks: effects,
            led_tables: led_tables,
            color_tables: color_tables
        };
        return JSON.stringify(data);
    }

    function timelineFromJson(json) {
        var data = JSON.parse(json);
        var item;
        var idx;

        // Set color tables
        var color_tables = data.color_tables;
        colorsTableModel.clear();
        for (idx in color_tables) {
            item = color_tables[idx];
            colorsTableModel.append({name: idx, colors: item});
        }

        // Set led tables
        var led_tables = data.led_tables;
        ledTableModel.clear();
        for (idx in led_tables) {
            item = led_tables[idx];
            ledTableModel.append({name: idx, leds: JSON.stringify(item).slice(1, -1)});
        }

        // Set the timeline
        timelineModel.clear();
        generateEmptyTimeline();
        nextGroupId = 1;

        for (var track_idx=0; track_idx<4; track_idx++) {
            var items = data.tracks[track_idx];
            for (idx in items) {
                item = items[idx];
                var trackEntry = timelineModel.get(item._ticks).tracks.get(track_idx);
                var effect = {};
                for (var key in item) {
                    if (key !== "start_time" && key !== "end_time" && key !== "props") {
                        effect[key] = item[key];
                    }
                }
                trackEntry.effect = effect;
                trackEntry.props = item.props;
                trackEntry.duration = item._duration;
                trackEntry.active = true;
                trackEntry.groupid = nextGroupId++;
                trackEntry.first = true;
                // Fill to the right
                for (idx=item._ticks+1; idx<item._ticks+item._duration; idx++) {
                    var entry = timelineModel.get(idx).tracks.get(track_idx);
                    entry.active = true;
                    entry.groupid = trackEntry.groupid;
                    entry.first = false;
                }
            }
        }
    }

    LedStripDriver {
        id: api
        ledStrip: ledStrip
        onCurrentTimeChanged: {
            currentTimeTick = Math.floor(currentTime * 10);
            if (api.started && !api.paused) {
                timelinesView.contentX = currentTimeTick * timeline_cell_width;
            }
        }
        onPausedChanged: {
            pauseButton.checked = paused;
        }
        onStartedChanged: {
            if (!started) {
                currentTimeTick = 0;
            } else {
                loadFromJson(timelineAsJson());
            }
        }

        Component.onCompleted: {
            init();
            generateEmptyTimeline();
        }
    }
    function generateEmptyTimeline() {
        var ticks = 0;
        for (var minutes=0; minutes<4; minutes++) {
            for (var seconds=0; seconds<60; seconds++) {
                for (var tenths=0; tenths<10; tenths++) {
                    var tracks = listModelComponent.createObject(timelineModel);
                    for (var t=0; t < num_tracks; t++) {
                        tracks.append({active:false, first: false, groupid: 0, effect: {name: "", cycles: 1, inverted_easing: false, blend_mode: "", easing: "", colors: "", leds: ""}, props: {}, duration: 0});
                    }
                    timelineModel.append({ticks: ticks, minutes: minutes, seconds: seconds, tenths: tenths, tracks: tracks});
                    ticks++;
                }
            }
        }
    }

    ColumnLayout {
        id: topRow
        anchors.top: parent.top
        anchors.left: parent.left
        width: height
        anchors.bottom: scroll.top
        anchors.bottomMargin: 10
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.top: parent.top
            anchors.left: parent.left
            color: "lightgray"
            LedStrip {
                id: ledStrip
                anchors.fill: parent
                onWidthChanged: update()
                onHeightChanged: update()
            }
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Button {
                id: startButton
                text: fa.icons.fa_play_circle
                font: fa.font
                checkable: true
                ButtonGroup.group: pausePlayGroup
            }
            Button {
                id: pauseButton
                text: fa.icons.fa_pause_circle
                font: fa.font
                checkable: true
                ButtonGroup.group: pausePlayGroup
            }
            Button {
                id: stopButton
                text: fa.icons.fa_stop_circle
                font: fa.font
                checkable: true
                checked: true
                ButtonGroup.group: pausePlayGroup
            }
            ButtonGroup {
                id: pausePlayGroup
                onCheckedButtonChanged: {
                    if (checkedButton == startButton) {
                        if (api.paused) {
                            api.paused = false;
                        } else {
                            api.started = true;
                        }
                    } else if (checkedButton == pauseButton) {
                        if (!api.started) {
                            api.started = true;
                        }
                        api.paused = true;
                    } else if (checkedButton == stopButton) {
                        api.paused = false;
                        api.started = false;
                    }
                }
            }
            Item {
                Layout.fillWidth: true
                height: startButton.height
                Button {
                    id: loadButton
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: saveButton.left
                    text: fa.icons.fa_folder_open
                    font: fa.font
                    width: height
                    onClicked: loadFile.open()
                }
                Button {
                    id: saveButton
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    text: fa.icons.fa_save
                    font: fa.font
                    width: height
                    onClicked: saveFile.open()
                }
            }
        }
    }

    Item {
        anchors.top: parent.top
        anchors.bottom: scroll.top
        anchors.right: parent.right
        anchors.left: topRow.right
        anchors.leftMargin: 2
        anchors.bottomMargin: 10
        ToolBar {
            id: tabs
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            contentHeight: propertiesTabButton.implicitHeight
            RowLayout {
                spacing: 5
                ToolButton {
                    id: propertiesTabButton
                    text: "Properties"
                    checkable: true
                    checked: true
                    ButtonGroup.group: tabsGroup
                    property var component: properties
                }
                ToolButton {
                    text: "Color Tables"
                    checkable: true
                    ButtonGroup.group: tabsGroup
                    property var component: colorTables
                }
                ToolButton {
                    text: "LED Tables"
                    checkable: true
                    ButtonGroup.group: tabsGroup
                    property var component: ledTable
                }
                ButtonGroup {
                    id: tabsGroup
                    onCheckedButtonChanged: {
                        stack.pop()
                        stack.push(checkedButton.component)
                    }
                }
            }
        }
        Rectangle {
            anchors.top: tabs.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 1
            border.color: "gray"
            border.width: 1
            StackView {
                id: stack
                anchors.fill: parent
                anchors.margins: 2
                initialItem: properties
                clip: true
            }
        }
        Component {
            id: properties
            Item {
                id: propertiesContainer
                property bool editingActive: false
                Text {
                    anchors.fill: parent
                    anchors.topMargin: 30
                    visible: !editingActive
                    font.pointSize: 18
                    horizontalAlignment: Text.AlignHCenter
                    text: "No Effect Selected"
                }

                Rectangle {
                    anchors.fill: parent
                    opacity: editingActive ? 1 : 0
                    GridLayout {
                        id: propertiesGrid
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 4
                        height: 220
                        columns: 2
                        rowSpacing: 2
                        Text {
                            id: trackLabel
                        }
                        Text {
                            id: timingLabel
                        }

                        Text {
                            text: "Effect:"
                        }
                        ComboBox {
                            id: effectsCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: {
                                commitProperties();
                                loadProps();
                            }
                        }

                        Text {
                            text: "Repeat Cycles:"
                        }
                        ComboBox {
                            id: cyclesCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: commitProperties()
                            model: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
                        }

                        Text {
                            text: "Easing Function:"
                        }
                        ComboBox {
                            id: easingCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: commitProperties()
                        }

                        Text {
                            text: "Easing Mode:"
                        }
                        ComboBox {
                            id: easingModeCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: commitProperties()
                            model: ["Normal", "Inverted"]
                        }

                        Text {
                            text: "Blend Mode:"
                        }
                        ComboBox {
                            id: blendModeCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: commitProperties()
                        }


                        Text {
                            text: "LEDs Table:"
                        }
                        ComboBox {
                            id: colorsCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: commitProperties()
                        }

                        Text {
                            text: "Color Table:"
                        }
                        ComboBox {
                            id: ledsCombo
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onActivated: commitProperties()
                        }

                        Text {
                            text: "Effect Properties:"
                        }
                    }
                    ListView {
                        id: propsListView
                        anchors.top: propertiesGrid.bottom
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 2
                        delegate: RowLayout {
                            height: 30
                            Rectangle {
                                Layout.fillHeight: true
                                width: 100
                                Text {
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignLeft
                                    text: name + ":"
                                }
                            }

                            TextField {
                                Layout.fillWidth: true
                                height: 15
                                selectByMouse: true
                                text: value
                                onEditingFinished: {
                                    value = text;
                                    commitProperties();
                                }
                                background: Rectangle {
                                    implicitWidth: 200
                                    implicitHeight: 20
                                    color: enabled ? "transparent" : "#353637"
                                    border.color: enabled ? "#21be2b" : "transparent"
                                }
                            }
                        }
                    }
                }
                property int selectedItemNotifier: selectedTickUpdateNotifier
                property var selectedEffectInstance
                onSelectedItemNotifierChanged: {
                    if (selectedTick != -1 && selectedTimeline != -1) {
                        // Item selected in timeline
                        var timelineEntry = timelineModel.get(selectedTick);
                        selectedEffectInstance = timelineEntry.tracks.get(selectedTimeline);

                        // Set the informational text
                        trackLabel.text = "Track " + (selectedTimeline + 1);
                        var start_time = "" + timelineEntry.minutes + ":" + leftpad(timelineEntry.seconds, 2) + "." + timelineEntry.tenths;
                        var durationTicks = selectedEffectInstance.duration;
                        var minutes = Math.floor(durationTicks/600);
                        var seconds = Math.floor(durationTicks/10) - (minutes*60);
                        var tenths = durationTicks - (((minutes * 60) + seconds) * 10);
                        var duration = "" + minutes + ":" + seconds + "." + tenths;
                        timingLabel.text = "Start Time: " + start_time + "     Duration: " + duration;

                        // Populate the fields
                        var effect = selectedEffectInstance.effect;
                        var props = selectedEffectInstance.props;
                        setCombo(effectsCombo, effect.name);
                        setCombo(cyclesCombo, effect.cycles);
                        setCombo(easingCombo, effect.easing);
                        setCombo(colorsCombo, effect.colors);
                        setCombo(ledsCombo, effect.leds);

                        easingModeCombo.currentIndex = effect.inverted_easing ? 1 : 0

                        // Track 0 can only have Normal blend mode
                        if (selectedTimeline == 0) {
                            blendModeCombo.currentIndex = 0;
                            blendModeCombo.enabled = false;
                        } else {
                            setCombo(blendModeCombo, effect.blend_mode);
                            blendModeCombo.enabled = true;
                        }

                        // Mark as actively editing
                        editingActive = true;
                        loadProps();
                        commitProperties();
                    } else {
                        editingActive = false;
                    }
                }
                function loadProps() {
                    var props = api.parametersForEffect(effectsCombo.currentText);
                    var key;
                    if (editingActive) {
                        var values = selectedEffectInstance.props;
                        for (key in props) {
                            var value = values[key];
                            if (value !== undefined) {
                                props[key] = value;
                            }
                        }
                    }
                    propsListView.model = listModelComponent.createObject(propsListView);
                    for (key in props) {
                        propsListView.model.append({name: key, value: props[key].toFixed(4)});
                    }
                }

                function setCombo(combo, value) {
                    if (value !== "") {
                        combo.currentIndex = combo.find(value);
                    } else {
                        combo.currentIndex = 0;
                    }
                }

                function commitProperties() {
                    if (editingActive) {
                        // Commit changes to model
                        var effect = {
                            name: effectsCombo.currentText,
                            cycles: parseInt(cyclesCombo.currentText),
                            easing: easingCombo.currentText,
                            inverted_easing: easingModeCombo.currentIndex == 1,
                            blend_mode: blendModeCombo.currentText,
                            colors: colorsCombo.currentText,
                            leds: ledsCombo.currentText
                        };
                        selectedEffectInstance.effect = effect;

                        var props = {};
                        for (var idx=0; idx<propsListView.model.count; idx++) {
                            var item  = propsListView.model.get(idx);
                            props[item.name] = parseFloat(item.value);
                        }
                        selectedEffectInstance.props = props;
                    }
                }

                Component.onCompleted: {
                    colorsCombo.model = Qt.binding(function(){
                        var colors = ["white", "off", "superwhite", "red", "green", "blue"];
                        for (var idx=0; idx<colorsTableModel.count; idx++) {
                            colors.push(colorsTableModel.get(idx).name);
                        }
                        return colors;
                    });
                    ledsCombo.model = Qt.binding(function(){
                        var leds = ["all", "J", "G"];
                        for (var idx=0; idx<ledTableModel.count; idx++) {
                            leds.push(ledTableModel.get(idx).name);
                        }
                        return leds;
                    });
                    // Not sure why the following two are needed, but the combo boxes don't populate without copying to a new array
                    effectsCombo.model = Qt.binding(function(){
                        var effects = [];
                        for (var idx in api.effects) {
                            effects.push(api.effects[idx]);
                        }
                        return effects;
                    });
                    easingCombo.model = Qt.binding(function(){
                        var funcs = [];
                        for (var idx in api.easingFunctions) {
                            funcs.push(api.easingFunctions[idx]);
                        }
                        return funcs;
                    });
                    blendModeCombo.model = Qt.binding(function(){
                        var modes = [];
                        for (var idx in api.blendModes) {
                            modes.push(api.blendModes[idx]);
                        }
                        return modes;
                    });
                }
            }
        }
        Component {
            id: ledTable
            ListEditor {
                id: ledListView
                model: ledTableModel
                delegate: delegateComponentLeds
                onInsert: function () {
                    return {name: "New LED Table", leds: ""};
                }

                Component {
                    id: delegateComponentLeds
                    Rectangle {
                        width: stack.width
                        height: 50
                        border.color: "lavender"
                        border.width: 1
                        Item {
                            id: checkBoxContainerLeds
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            width: 50
                            CheckBox {
                                anchors.centerIn: parent
                                width: 15
                                height: 15
                                onCheckStateChanged: _currentlySelected = toggleSelected(checkState > 0)
                            }
                        }

                        TextField {
                            id: ledListName
                            anchors.top: parent.top
                            anchors.left: checkBoxContainerLeds.right
                            height: parent.height * 0.5
                            width: (parent.width - checkBoxContainerLeds.width) * 0.5
                            text: name
                            font.pointSize: 10
                            selectByMouse: true
                            onEditingFinished: name = text
                        }
                        TextField {
                            id: listOfLeds
                            anchors.top: ledListName.bottom
                            anchors.left: checkBoxContainerLeds.right
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            font.pointSize: 10
                            text: leds
                            onEditingFinished: leds = text
                        }
                    }
                }
            }
        }
        Component {
            id: colorTables
            ListEditor {
                id: ledListView
                model: colorsTableModel
                delegate: delegateComponentColors
                onInsert: function () {
                    return {name: "New Color Table", colors: []};
                }

                Component {
                    id: delegateComponentColors
                    Rectangle {
                        width: ledListView.width
                        height: 50
                        border.color: "lavender"
                        border.width: 1
                        Item {
                            id: checkBoxContainerColors
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            width: 50
                            CheckBox {
                                anchors.centerIn: parent
                                width: 15
                                height: 15
                                onCheckStateChanged: _currentlySelected = toggleSelected(checkState > 0)
                            }
                        }

                        TextField {
                            id: colorListName
                            anchors.top: parent.top
                            anchors.left: checkBoxContainerColors.right
                            height: parent.height * 0.5
                            width: (parent.width - checkBoxContainerColors.width) * 0.5
                            text: name
                            font.pointSize: 10
                            selectByMouse: true
                            onEditingFinished: name = text
                        }
                        Button {
                            id: addNewColorButton
                            anchors.left: checkBoxContainerColors.right
                            anchors.top: colorListName.bottom
                            anchors.topMargin: 1
                            anchors.leftMargin: 1
                            width: 20
                            height: 20
                            font: fa.font
                            text: fa.icons.fa_plus_circle
                            onClicked: colors.append({h: 0, s: 1.0, l: 0.5, w: 0});
                        }
                        ListView {
                            id: colorsList
                            anchors.top: colorListName.bottom
                            anchors.left: addNewColorButton.right
                            anchors.right: parent.right
                            anchors.leftMargin: 2
                            anchors.topMargin: 2
                            orientation: Qt.LeftToRight
                            spacing: 2
                            clip: true
                            height: 20
                            model: colors
                            delegate: Rectangle {
                                width: 20
                                height: 20
                                color: Qt.hsla(h / 360.0, s, l, 1)
                                border.width: 1
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        hslEditPopup.model = colorsList.model
                                        hslEditPopup.index = index
                                        hslEditPopup.open()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ScrollView {
        id: scroll
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: (num_tracks * 40) + 30
        ScrollBar.horizontal: ScrollBar {
            parent: scroll
            x: scroll.leftPadding
            y: scroll.height - height
            width: scroll.availableWidth
            active: scroll.ScrollBar.vertical.active
            anchors.left: timelinesView.left
            anchors.right: timelinesView.right
            anchors.bottom: timelinesView.top
        }

        ListView {
            id: timelinesView
            anchors.fill: parent
            orientation:  Qt.Horizontal
            height: 30
            model: timelineModel
            delegate: Rectangle {
                property int currentTick: ticks
                color: timelineColumn.containsMouse ? "honeydew" : "whitesmoke"
                width: timeline_cell_width
                height: scroll.height
                border.color: timelineColumn.containsMouse ? "black" : "gray"
                border.width: 1
                MouseArea {
                    id: timelineColumn
                    anchors.fill: parent
                    hoverEnabled: true
                    Rectangle {
                        id: header
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 30
                        color: tenths == 0 ? "gray" : "lightgray"
                        border.width: currentTimeTick == ticks ? 2 : 1
                        border.color: currentTimeTick == ticks ? "red" : "darkgray"
                        Text {
                            anchors.fill: parent
                            text: "" + minutes + ":" + leftpad(seconds, 2) + "." + tenths + "\n" + ticks
                            font.bold: tenths == 0 || tenths == 5
                            font.pointSize: 13
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: api.seek(ticks * 0.25);
                        }
                    }
                    ColumnLayout {
                        anchors.top: header.bottom
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        spacing: 0
                        Repeater {
                            id: repeater
                            model: tracks
                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                MouseArea {
                                    id: timelineCell
                                    hoverEnabled: true
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                                    onClicked: {
                                        var cell;
                                        var lastTick;
                                        var firstTick = ticks;
                                        if (mouse.button === Qt.LeftButton) {
                                            if (!active) {
                                                active = true;
                                                // Entry is now added
                                                groupid = nextGroupId++;
                                                first = true;
                                                duration = fillRight(ticks, groupid, index);
                                            } else {
                                                firstTick = findFirst(ticks, index);
                                                console.log("First after insertion:", firstTick);
                                            }
                                            cell = timelineModel.get(firstTick).tracks.get(index);
                                            console.log("SELECTED", groupid, cell.groupid);
                                            // Select
                                            selectedGroupId = groupid;
                                            selectedTick = firstTick;
                                            selectedTimeline = index;
                                        } else if (mouse.button === Qt.RightButton && active) {
                                            if (active) {
                                                // Remove all cells of the same group id to the right
                                                clearRight(ticks, groupid, index);
                                                groupid = 0;
                                                // Update duration on the left
                                                firstTick = findFirst(ticks-1, index);
                                                console.log("First, after deletion:", firstTick, ticks);
                                                cell = timelineModel.get(firstTick).tracks.get(index);
                                                cell.duration = ticks - firstTick;
                                                // Remove this cell
                                                active = false;
                                                if (firstTick !== ticks && cell.active) {
                                                    selectedGroupId = cell.groupid;
                                                    selectedTick = firstTick;
                                                    selectedTimeline = index;
                                                } else {
                                                    selectedGroupId = 0;
                                                    selectedTick = -1;
                                                    selectedTimeline = -1;
                                                }
                                            }
                                        }
                                        timelineUpdated();
                                    }
                                    Rectangle {
                                        anchors.fill: parent
                                        anchors.leftMargin: 2
                                        anchors.rightMargin: 2
                                        color: timelineCell.containsMouse ? "lightgray" : "transparent"
                                        Rectangle {
                                            id: effect
                                            anchors.fill: parent
                                            color: selectedGroupId == groupid ? Qt.hsla(((groupid * 40) % 360) / 360, 1, 0.2, 1) : Qt.hsla(((groupid * 40) % 360) / 360, 0.3, 0.7, 1)
                                            visible: active
                                            Text {
                                                anchors.top: parent.top
                                                anchors.left: parent.left
                                                width: parent.width * 0.5
                                                height: parent.height * 0.5
                                                text: selectedGroupId == groupid ? fa.icons.fa_check : fa.icons.fa_edit
                                                font: fa.font
                                                color: selectedGroupId == groupid ? "white" : "black"
                                                visible: first
                                            }
                                        }
                                    }
                                }

                                Rectangle {
                                    anchors.bottom: parent.bottom
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    height: 1
                                    color: "darkgray"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Popup {
        id: saveFile
        x: (window.width * 0.5) - (width * 0.5)
        y: (window.height * 0.5) - (height * 0.5)
        width: 340
        height: 150
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        Item {
            anchors.fill: parent
            anchors.margins: 10
            Text {
                id: saveFileLabel
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: saveFileName.bottom
                verticalAlignment: Text.AlignVCenter
                text: "File name: "
            }
            TextField {
                id: saveFileName
                anchors.left: saveFileLabel.right
                anchors.top: parent.top
                selectByMouse: true
                text: "ledjacket"
            }
            Text {
                anchors.left: saveFileName.right
                anchors.top: parent.top
                anchors.bottom: saveFileName.bottom
                verticalAlignment: Text.AlignVCenter
                text: ".json"
            }
            RowLayout {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                Text {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Save to home directory"
                }
                Text {
                    property bool checked: false
                    id: exportToHomeDir
                    text: checked ? fa.icons.fa_toggle_on : fa.icons.fa_toggle_off
                    font: fa.font
                    MouseArea {
                        anchors.fill: parent
                        onClicked: exportToHomeDir.checked = !exportToHomeDir.checked
                    }
                }
            }
            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                text: "Save File"
                onClicked: {
                    var json = timelineAsJson();
                    api.writeToFile(exportToHomeDir.checked, saveFileName.text + ".json", json);
                    saveFile.close()
                }
            }
        }
    }
    Popup {
        id: loadFile
        x: (window.width * 0.5) - (width * 0.5)
        y: (window.height * 0.5) - (height * 0.5)
        width: 340
        height: 150
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        Item {
            anchors.fill: parent
            anchors.margins: 10
            Text {
                id: openFileLabel
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: openFileName.bottom
                verticalAlignment: Text.AlignVCenter
                text: "File name: "
            }
            TextField {
                id: openFileName
                anchors.left: openFileLabel.right
                anchors.top: parent.top
                selectByMouse: true
                text: "ledjacket"
            }
            Text {
                anchors.left: openFileName.right
                anchors.top: parent.top
                anchors.bottom: openFileName.bottom
                verticalAlignment: Text.AlignVCenter
                text: ".json"
            }
            RowLayout {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                Text {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    text: "Load from home directory"
                }
                Text {
                    property bool checked: false
                    id: importFromHomeDir
                    text: checked ? fa.icons.fa_toggle_on : fa.icons.fa_toggle_off
                    font: fa.font
                    MouseArea {
                        anchors.fill: parent
                        onClicked: importFromHomeDir.checked = !importFromHomeDir.checked
                    }
                }
            }
            Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                text: "Load File"
                onClicked: {
                    var json = api.readFromFile(importFromHomeDir.checked, openFileName.text + ".json");
                    timelineFromJson(json);
                    loadFile.close()
                }
            }
        }
    }

    Popup {
        id: hslEditPopup
        property var model
        property int index: 0
        property var modelData
        x: (window.width * 0.5) - (width * 0.5)
        y: (window.height * 0.5) - (height * 0.5)
        width: 340
        height: 200
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        RowLayout {
            Rectangle {
                id: colorPreview
                width: 100
                height: 100
                border.width: 2
                border.color: "black"
            }
            ColumnLayout {
                Slider {
                    from: 0.0
                    to: 360.0
                    value: hslEditPopup.modelData !== undefined ? hslEditPopup.modelData.h : 0
                    onValueChanged: {
                        hslEditPopup.modelData.h = value
                        hslEditPopup.setColor();
                    }
                }
                Slider {
                    from: 0.0
                    to: 1.0
                    value: hslEditPopup.modelData !== undefined ? hslEditPopup.modelData.s : 0
                    onValueChanged: {
                        hslEditPopup.modelData.s = value
                        hslEditPopup.setColor();
                    }
                }
                Slider {
                    from: 0.0
                    to: 1.0
                    value: hslEditPopup.modelData !== undefined ? hslEditPopup.modelData.l : 0
                    onValueChanged: {
                        hslEditPopup.modelData.l = value
                        hslEditPopup.setColor();
                    }
                }
                Slider {
                    from: 0.0
                    to: 1.0
                    value: hslEditPopup.modelData !== undefined ? hslEditPopup.modelData.w : 0
                    onValueChanged: hslEditPopup.modelData.w = value
                }
            }
        }
        onOpened: {
            hslEditPopup.modelData = hslEditPopup.model.get(hslEditPopup.index);
            setColor();
        }
        function setColor() {
            var color = hslEditPopup.modelData;
            if (color !== undefined) {
                colorPreview.color = Qt.hsla(color.h / 360.0, color.s, color.l, 1);
            } else {
                colorPreview.color = "transparent";
            }
        }
    }
    function clearRight(startTick, searchGroupId, currentTimeline) {
        var currentTick = startTick + 1;
        while (currentTick < timelineModel.count) {
            var cell = timelineModel.get(currentTick).tracks.get(currentTimeline);
            if (cell.groupid === searchGroupId && cell.active) {
                cell.groupid = 0;
                cell.active = false;
                cell.first = false;
                ++currentTick;
            } else {
                break;
            }
        }
        return currentTick - startTick;
    }
    function fillRight(startTick, newGroupId, currentTimeline) {
        var currentTick = startTick + 1;
        while (currentTick < timelineModel.count) {
            var cell = timelineModel.get(currentTick).tracks.get(currentTimeline);
            if (cell.groupid === 0 && !cell.active) {
                cell.groupid = newGroupId;
                cell.active = true;
                cell.first = false;
                ++currentTick;
            } else {
                break;
            }
        }
        return currentTick - startTick;
    }

    function findFirst(startTick, currentTimeline) {
        var currentTick = startTick;
        var expectedGroupId = undefined;
        while (currentTick >= 0) {
            console.log("Checking", currentTick, expectedGroupId);
            var cell = timelineModel.get(currentTick).tracks.get(currentTimeline);
            if (cell.active && (expectedGroupId == undefined || expectedGroupId === cell.groupid)) {
                expectedGroupId = cell.groupid;
                currentTick--;
                console.log("Moving backwards", currentTick, cell.groupid);
            } else {
                console.log("Returning", currentTick+1);
                return currentTick+1;
            }
        }
        console.log("Returning", currentTick);
        return currentTick;
    }

    function timelineUpdated() {
        selectedTickUpdateNotifier = !selectedTickUpdateNotifier;
    }

    function invertColor(color) {
        return Qt.rgba(1 - color.r, 1 - color.g, 1 - color.b, color.a);
    }

    function leftpad(num, size) {
        var s = num+"";
        while (s.length < size) s = "0" + s;
        return s;
    }
}
