docker run -d \
  -p 8080:8080 \
  -p 50000:50000 \
  -e JAVA_OPTS="-Djenkins.install.runSetupWizard=false" \
  -v jenkins_home:/var/jenkins_home \
  jenkins/jenkins:lts