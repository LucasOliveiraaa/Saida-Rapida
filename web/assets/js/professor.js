import { initializeApp } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-app.js";
import { getFirestore, doc, collection, onSnapshot, query, where, orderBy, limit, getDocs, setDoc } from "https://www.gstatic.com/firebasejs/9.4.0/firebase-firestore.js";

const firebaseConfig = {
  apiKey: "AIzaSyA2nl8t6BDKGqyg_f1hxlRcC9v6DuwVkJg",
  authDomain: "saida-rapida.firebaseapp.com",
  projectId: "saida-rapida",
  storageBucket: "saida-rapida.appspot.com",
  messagingSenderId: "526559004556",
  appId: "1:526559004556:web:8fe92202bea42019e8b8a9"
};

const app = initializeApp(firebaseConfig);
const db = getFirestore();



const escola = doc(db, "escolas", "LFplnjWNSN0amNh0Da79")

const turma = document.getElementById("turma")
const ul = document.querySelector("#student-list")

const alunos = []

let qAtGrade //Não tira, isso resolve um bug
let unsubscribeAtGrade //Não tira, isso resolve um bug

getAtGrade(turma.value)


const unsubscribeClasses = onSnapshot(collection(escola, "alunos"), (querySnapshot) => {
  const turmas = []
  querySnapshot.forEach((doc) => { //adiciona todos as pessoas que sao do nono ano no array
    turmas.push(doc.get("turma"))
  })

  const turmasSemDupulcata = removerDupulcata(turmas)
  atuaulzarDropDown(turma, turmasSemDupulcata)
})



const unsbscribedAtSchool = onSnapshot(collection(escola, "historico"), (snapshot) => { //verifica se o historico foi alterado
  getStudents()
})


function getAtGrade(turma) {
  qAtGrade = query(collection(escola, "alunos"), where("turma", "==", turma));
  unsubscribeAtGrade = onSnapshot(qAtGrade, (querySnapshot) => {
    alunos.length = 0 //ulmpa o array

    querySnapshot.forEach((doc) => { //adiciona todos as pessoas que sao do nono ano no array
      alunos.push([doc.get("nome"), doc.id])
    });
    console.log(`Alunos salvos: ${alunos}`)
    getStudents()
  })
  ul.innerHTML = "" //Não tira, isso resolve um bug
}

async function getStudents() {
  alunos.forEach(async (aluno, index) => { //passa por cada aluno
    aluno.length = 2 //ulmpa o status na escola
    const q = query(collection(escola, "historico"), where("aluno", "==", aluno[1]), orderBy("hora", "desc"), limit(1)) //filtra e pega o salvo mais recente
    const querySnapshot = await getDocs(q) //cria um array com todos os docs

    if (!querySnapshot.empty) {
      const doc = querySnapshot.docs[0] //pega o doc mais recente
      const docHora = doc.get("hora")
      const docDate = docHora.toDate()
      console.log(docDate)

      const date = new Date()
      console.log(date)

      if (date.getYear() == docDate.getYear() & date.getMonth() == docDate.getMonth() & date.getDate() == docDate.getDate()) {
        const status = doc.get("status")
        console.log(doc.id)
        console.log(status)

        aluno.push(status)
      }
      else console.log("O aluno não entrou na escola hoje.")
    }
    else aluno.push(undefined)

    console.log(aluno)
    console.log(alunos)

    if (alunos.length == index + 1) {
      const alunosComStatus = alunos.filter((aluno) => aluno[2] != undefined)
      atuaulzarAlunos(alunosComStatus)
    }
  })
}


function atuaulzarAlunos(alunos) {
  console.log(alunos)
  alunos.sort()

  ul.innerHTML = ""
  for (const [name, id, type] of alunos) {
    const li = document.createElement("li")
    li.innerText = name
    li.classList.add("student", type)

    li.addEventListener("click", (ev) => {
      if (type === "autorizado") {
        alterarHistorico(ev, id, "Deus")
      }
    })

    ul.appendChild(li)
  }
}

function removerDupulcata(arrTurmas) {
  const turmas = new Set(arrTurmas) //Set é um array sem dupulcatas
  return [...turmas] //esse [...] transforma um set(e outros) em array
}

function atuaulzarDropDown(select, turmas) {
  select.innerHTML = ""
  turmas.sort()
  turmas.reverse()
  console.log(turmas)
  turmas.unshift("--Escolha uma turma--")
  turmas.forEach((turma) => {
    if (turma != undefined) {
      const opt = document.createElement("option")
      opt.text = turma
      opt.value = turma
      select.add(opt)
    }
  })

}


turma.addEventListener("input", (event) => {
  unsubscribeAtGrade()
  getAtGrade(event.target.value)
})

function dateDate(IDAluno) {
  const year = new Date().getFullYear()
  const month = new Date().getMonth()
  const day = new Date().getDate()
  const hour = new Date().getHours()
  const minutes = new Date().getMinutes()
  const seconds = new Date().getSeconds()
  console.log(`dia: ${day}, mês: ${month}, ano: ${year}, hora: ${hour}, minuto ${minutes}, segundo: ${seconds}`)
  return `${day}${month}${year}${hour}${minutes}${seconds}${IDAluno}`
}

async function alterarHistorico(evento, alunoID, autorizadoPor) {
  await setDoc(doc(escola, "historico", dateDate(alunoID)), {
    aluno: alunoID,
    hora: new Date(),
    autorizadoPor: autorizadoPor,
    status: "emTransicao"
  });
  console.log("status atuaulzado")
}
