const camera = document.querySelector("#camera")
const id_input = document.querySelector("#id")
const save_button = document.querySelector("#save")

const reconeye = document.querySelector("#reconeye")
const bancodedados = document.querySelector("#database")
const fps = document.querySelector("#fps")

let main_data = null

save_button.addEventListener("click", async () => {
    const id = id_input.value

    await glob.saveNewDescriptor(id, main_data)

    console.log("Salvo")
})

glob.receive(async (main, id, data) => {
    if (id !== "" && !main)
        return;

    main_data = data
}, camera, reconeye, bancodedados, fps)