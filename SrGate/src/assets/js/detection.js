const camera = document.querySelector("#camera")
const list = document.querySelector("#detections")

const reconeye = document.querySelector("#reconeye")
const bancodedados = document.querySelector("#database")
const fps = document.querySelector("#fps")

const studentsInUi = new Set()
async function addStudentToUi(id) {
    if (studentsInUi.has(id)) {
        return;
    }

    const student = await glob.getStudent(id)

    if (!student) {
        console.error(`Error: Failed to get student with id: ${id}`)
        return;
    }

    studentsInUi.add(id)

    const li = document.createElement("li")
    li.classList.add("detection")
    li.innerHTML = `<div class="title">
                            <h3>${student.nome}</h3>
                            <p class="class">${student.turma}</p>
                        </div>
                        <div class="buttons">
                            <button class="btn btn-success" id="enter">Entrar</button>
                            <button class="btn btn-error" id="remove">Remover</button>
                        </div>`
    const enter = li.querySelector("#enter")
    const remove = li.querySelector("#remove")
    enter.addEventListener("click", async () => {
        await glob.getInside(id)
        li.remove()
        studentsInUi.delete(id)
    })
    remove.addEventListener("click", async () => {
        li.remove()
        studentsInUi.delete(id)
    })

    list.appendChild(li)
}

glob.receive(async (main, id, _) => {
    if (id === "")
        return;

    await addStudentToUi(id);
}, camera, reconeye, bancodedados, fps)