freeStyleJob('Disk Space Check') {
    steps {
        shell('df')
    }
}

freeStyleJob('Daily Dose of Satisfaction') {
    parameters {
        stringParam('NAME', 'user', 'Name used in greeting')
    }

    steps {
        shell('echo "Hello dear ${NAME}!"')
    }

    steps {
        shell('date')
    }

    steps {
        shell('echo "This is your DDoS number ${BUILD_NUMBER}."')
    }
}
